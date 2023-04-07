#include "simple_http_server.h"

namespace simpleHttpServer
{
    SimpleHTTPServer::SimpleHTTPServer(const std::string &address,
                                       const std::string &port)
        : _acceptor(_ioc, {tcp::v4(), static_cast<unsigned short>(std::stoi(port))}),
          _sipeto(sipeto::Sipeto())
    {
        // createSession();
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    void SimpleHTTPServer::start()
    {
        createSession();
        _ioc.run();
    }

    void SimpleHTTPServer::runSessionMethod()
    {
        for (const auto &session : _sessions)
        {
            session->acceptConnections();
        }
    }

    void SimpleHTTPServer::createSession()
    {
        // Create a new socket for the incoming connection
        tcp::socket socket{_ioc};

        // Accept a connection
        _acceptor.accept(socket);

        // Create a new Session instance with the connected socket and the _sipeto instance
        auto session = std::make_shared<Session>(std::move(socket), _sipeto, _acceptor);

        // Store the session in the _sessions vector
        _sessions.push_back(session);

        // Start the session (assuming the start() method of Session class handles necessary operations)
        session->start();
    }

    SimpleHTTPServer::Session::Session(tcp::socket socket, sipeto::Sipeto &sipeto, tcp::acceptor &acceptor)
        : _socket(std::move(socket)), _sipeto(sipeto), _acceptor(acceptor) {}

    void SimpleHTTPServer::Session::start()
    {
        readRequest();
    }

    size_t SimpleHTTPServer::writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        size_t realsize = size * nmemb;
        std::stringstream *responseBuffer = (std::stringstream *)userdata;
        responseBuffer->write(ptr, realsize);
        return realsize;
    }

    /// @brief  set up a webHookUrl for Telegram bot
    /// @param none
    /// @return none
    /* NOTE: Once a webHookUrl has been set up for
     * a Telegram bot, it does not need to be set
     * up again unless there is a change the URL or disable the webHookUrl.
     * TODO: check if the webHookUrl is already set */
    void SimpleHTTPServer::setwebHookUrl()
    {
        spdlog::info("Setting up webHookUrl...");
        std::string url = _sipeto.getFromConfigMap("endpoint") + _sipeto.getFromConfigMap("token") + "/setwebHookUrl?url=" + _sipeto.getFromConfigMap("webHookUrl") + "&webhook_use_self_signed=true";
        spdlog::info("url: {}", url);

        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &SimpleHTTPServer::writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_responseBuffer);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
            }
            else
            {
                // Check if webHookUrl is already set
                std::string responseString = _responseBuffer.str();
                Json::Value responseJson;
                Json::Reader reader;
                if (!reader.parse(responseString, responseJson))
                {
                    spdlog::error("Failed to parse JSON response from getwebHookUrlInfo.");
                }
                else
                {
                    bool webHookUrlIsSet = responseJson["result"].asBool(); // == _sipeto.getFromConfigMap("webHookUrl");

                    if (webHookUrlIsSet)
                    {
                        spdlog::info("{}.", responseJson["description"].asString());
                        return;
                    }
                }
            }
        }
        else
        {
            spdlog::error("curl_easy_init() failed: {}", curl_easy_strerror(res));
        }
    }

    void SimpleHTTPServer::Session::acceptConnections()
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

    void SimpleHTTPServer::Session::readRequest()
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

    void SimpleHTTPServer::Session::handleRequest()
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

    void SimpleHTTPServer::Session::writeResponse()
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

    void SimpleHTTPServer::Session::processTelegramUpdate()
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
