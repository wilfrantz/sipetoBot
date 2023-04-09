#include "sipeto.h"
#include "simple_http_server.h"

namespace simpleHttpServer
{

    SimpleHttpServer::SimpleHttpServer(const std::string &address,
                                       const std::string &port)
        : _sipeto(new sipeto::Sipeto()),
          _port(_sipeto->getFromConfigMap("port")),
          _address(_sipeto->getFromConfigMap("address")),
          _acceptor(_ioc, {tcp::v4(), static_cast<unsigned short>(std::stoi(port))})
    {
        // createSession();
        curl_global_init(CURL_GLOBAL_DEFAULT);
        _acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    }

    void SimpleHttpServer::start()
    {
        createSession();
        _ioc.run();
    }

    void SimpleHttpServer::runSessionMethod()
    {
        for (const auto &session : _sessions)
        {
            session->acceptConnections();
        }
    }

    void SimpleHttpServer::createSession()
    {
        // Create a new socket for the incoming connection
        tcp::socket socket{_ioc};

        // Accept a connection
        _acceptor.accept(socket);

        // Create a new Session instance with the connected socket and the _sipeto instance
        auto session = std::make_shared<Session>(std::move(socket), *_sipeto, _acceptor);

        // Store the session in the _sessions vector
        _sessions.push_back(session);

        // Start the session (assuming the start() method of Session class handles necessary operations)
        session->start();
    }

    SimpleHttpServer::Session::Session(tcp::socket socket, sipeto::Sipeto &sipeto, tcp::acceptor &acceptor)
        : _socket(std::move(socket)), _sipeto(sipeto), _acceptor(acceptor) {}

    void SimpleHttpServer::Session::start()
    {
        readRequest();
    }

    size_t SimpleHttpServer::writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        size_t realsize = size * nmemb;
        std::stringstream *responseBuffer = (std::stringstream *)userdata;
        responseBuffer->write(ptr, realsize);
        return realsize;
    }

    /// @brief  set up a webHookUrl for Telegram bot
    /// @param none
    /// @return none
    /// NOTE: webHookUrl needs to be set up only once.
    void SimpleHttpServer::setwebHookUrl()
    {
        spdlog::info("Setting up webHookUrl...");
        if (!_sipeto)
        {
            spdlog::error("Cannot set webHookUrl: Sipeto object is not initialized");
            return;
        }

        // set up the webHook url string.
        std::string url = _sipeto->getFromConfigMap("endpoint");
        url += _sipeto->getFromConfigMap("token");
        url += "/setwebHookUrl?url=";
        url += _sipeto->getFromConfigMap("webHookUrl");
        url += "&webhook_use_self_signed=true";

        CURL *curl = curl_easy_init();
        if (!curl)
        {
            spdlog::error("Failed to initialize curl");
            return;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &SimpleHttpServer::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_responseBuffer);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
        }
        else
        {
            handleSetWebHookUrlResponse();
        }
    }

    /// @brief process the response from setwebHookUrl
    /// @param none
    /// @return none
    void SimpleHttpServer::handleSetWebHookUrlResponse()
    {
        std::string responseString = _responseBuffer.str();
        Json::Value responseJson;
        Json::Reader reader;
        if (!reader.parse(responseString, responseJson))
        {
            spdlog::error("Failed to parse JSON response from setwebHookUrl");
            return;
        }

        bool webHookUrlIsSet = responseJson["result"].asBool();
        if (webHookUrlIsSet)
        {
            spdlog::info("WebHookUrl set successfully: {}", responseJson["description"].asString());
        }
        else
        {
            spdlog::error("Failed to set WebHookUrl: {}", responseJson["description"].asString());
        }
    }

    void SimpleHttpServer::Session::acceptConnections()
    {
        _acceptor.async_accept(
            [this](std::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket), _sipeto, _acceptor)->start();
                }
                acceptConnections();
            });
    }

    void SimpleHttpServer::Session::readRequest()
    {
        auto self = shared_from_this();
        http::async_read(_socket, _buffer, _req,
                         [self](boost::system::error_code ec, std::size_t)
                         {
                             if (!ec)
                             {
                                 self->handleRequest();
                             }
                         });
    }

    void SimpleHttpServer::Session::handleRequest()
    {
        // Check if the incoming request is a Telegram bot update
        // if (_req.target() == _sipeto.getFromConfigMap("token"))
        if (_req.target() == "/" + _sipeto.getFromConfigMap("token"))
        {
            spdlog::info("Received Telegram bot update");
            // Process Telegram bot update
            processTelegramUpdate();
        }
        else
        {
            // Process request using Sipeto class (existing implementation)
            std::string result = _sipeto.processRequest(_req.body());

            // Generate the HTTP response (existing implementation)
            http::response<http::string_body> res{http::status::ok, _req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/plain");
            res.keep_alive(_req.keep_alive());
            res.body() = result;
            res.prepare_payload();
            writeResponse();
        }
    }

    void SimpleHttpServer::Session::writeResponse()
    {
        auto self = shared_from_this();
        http::async_write(_socket, _res,
                          [self](boost::system::error_code ec, std::size_t)
                          {
                              if (!ec && self->_res.need_eof())
                              {
                                  self->_socket.shutdown(tcp::socket::shutdown_send, ec);
                              }
                          });
    }

    void SimpleHttpServer::Session::processTelegramUpdate()
    {
        // Parse the request body as JSON
        Json::CharReaderBuilder builder;
        Json::Value update;
        std::string errors;
        std::istringstream ss(_req.body());
        if (!Json::parseFromStream(builder, ss, &update, &errors))
        {
            /// NOTE: If parsing fails, return an error response
            /// or handle it in a different way
            return;
        }

        // Pass the JSON data to the Sipeto class for further processing
        _sipeto.processTelegramUpdate(update);

        // Generate an HTTP response with status 200 OK
        http::response<http::empty_body> res{http::status::ok, _req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.keep_alive(_req.keep_alive());
        res.prepare_payload();
        writeResponse();
    }
}
