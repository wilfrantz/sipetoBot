#include "sipeto.h"
#include "simple_http_server.h"

namespace simpleHttpServer
{
    /// @brief simple http server constructor
    /// @param address of the server
    /// @param port to listen on
    SimpleHttpServer::SimpleHttpServer(sipeto::Sipeto &sipeto,
                                       const std::string &address,
                                       const std::string &port)
        : _sipeto(sipeto),
          _port(_sipeto.getFromConfigMap("port")),
          _address(_sipeto.getFromConfigMap("address"))
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);

        unsigned short port_number;
        try
        {
            port_number = static_cast<unsigned short>(std::stoi(_port));
        }
        catch (const std::invalid_argument &e)
        {
            spdlog::error("Invalid port number: {}", _port);
            throw std::invalid_argument("Invalid port number");
        }
        catch (const std::out_of_range &e)
        {
            spdlog::error("Port number out of range: {}", _port);
            throw std::out_of_range("Port number out of range");
        }

        // Try to bind a temporary socket to the specified IP address and port
        tcp::socket temp_socket{_ioc};

        try
        {
            spdlog::info("Creating endpoint...");
            _sipeto.getLogger()->debug("Address: {}", _address);
            _sipeto.getLogger()->debug("Port: {}", port_number);
            tcp::endpoint endpoint{boost::asio::ip::make_address(_address), port_number};
            boost::system::error_code ec;
            temp_socket.open(endpoint.protocol(), ec);
            if (ec)
            {
                throw std::runtime_error(std::string("Failed to open socket: ") + ec.message());
            }
            temp_socket.set_option(boost::asio::socket_base::reuse_address(true));
            temp_socket.bind(endpoint, ec);
            if (ec)
            {
                throw std::runtime_error(std::string("Failed to bind socket to endpoint: ") + ec.message());
            }
        }
        catch (const boost::system::system_error &e)
        {
            throw std::runtime_error(std::string("Failed to create endpoint: ") + e.what());
        }

        // Create the acceptor and set the reuse_address option
        _acceptor = std::make_unique<tcp::acceptor>(_ioc, tcp::endpoint(tcp::v4(), port_number));
        _acceptor->set_option(boost::asio::socket_base::reuse_address(true));
    }

    void SimpleHttpServer::start()
    {
        spdlog::info("SimpleHttpServer Start [{}:{}]", _address, _port);
        setwebHookUrl();
        createSession();
        _ioc.run();
    }

    void SimpleHttpServer::createSession()
    {
        spdlog::info("Creating session...");
        tcp::socket socket{_ioc};
        _acceptor->accept(socket);
        _sipeto.getLogger()->debug("Session created.");
        auto session = std::make_shared<Session>(std::move(socket), _sipeto, *_acceptor);
        _sessions.push_back(session);
        session->start();
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
        _sipeto.getLogger()->debug("Setting up webHookUrl...");
        std::string url = _sipeto.getFromConfigMap("endpoint");
        url += _sipeto.getFromConfigMap("token");
        url += "/setWebhook?url=";
        url += _sipeto.getFromConfigMap("webHookUrl");
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

        // Cleanup
        curl_easy_cleanup(curl);
    }

    /// @brief handle response from setWebHookUrl
    /// @param none
    /// @return none
    void SimpleHttpServer::handleSetWebHookUrlResponse()
    {
        std::string responseString = _responseBuffer.str();
        Json::Value responseJson;
        Json::Reader reader;
        if (!reader.parse(responseString, responseJson))
        {
            spdlog::error("Failed to parse JSON response from setWebHookUrl");
            return;
        }

        bool webHookUrlIsSet = responseJson["result"].asBool();
        if (webHookUrlIsSet)
        {
            // webHookUrl is set up successfully.
            _sipeto.getLogger()->debug("WebHookUrl: {}",
                                       responseJson["description"].asString());
        }
        else
        {
            // webHookUrl is not set
            _sipeto.getLogger()->debug("WebHookUrl: {}",
                                       responseJson["description"].asString());
            exit(1);
        }

        // Clear the response buffer for future use
        _responseBuffer.str(std::string());
        _responseBuffer.clear();
    }

    /*!SimpleHttpServer */

    /// @brief simple http server session constructor
    SimpleHttpServer::Session::Session(tcp::socket socket, sipeto::Sipeto &sipeto, tcp::acceptor &acceptor)
        : _socket(std::move(socket)), _sipeto(sipeto), _acceptor(acceptor) {}

    /// @brief Start the asynchronous operation
    void SimpleHttpServer::Session::start()
    {
        spdlog::info("Starting session...");
        readRequest();
    }

    /// @brief read incoming requests from Telegram bot
    /// @param none
    /// @return none
    void SimpleHttpServer::Session::readRequest()
    {
        spdlog::info("Reading request...");
        /// TODO: remove theses lines
        spdlog::info("HTTP method: {}", _req.method_string());
        spdlog::info("HTTP target: {}", _req.target());
        exit(1);


        auto self = shared_from_this();

        // Define a lambda function to handle errors during request reading
        auto onError = [self, this](const std::string &errorMessage)
        {
            spdlog::error(errorMessage);
            // Generate an HTTP response with status 400 Bad Request
            http::response<http::string_body> badRequestRes{http::status::bad_request, self->_req.version()};

            badRequestRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            badRequestRes.keep_alive(self->_req.keep_alive());
            badRequestRes.prepare_payload();
            this->writeResponse(badRequestRes);
        };

        http::async_read(_socket, _buffer, _req,
                         [self, onError, this](boost::system::error_code ec, std::size_t)
                         {
                             if (ec)
                             {
                                 onError("Failed to read request: " + ec.message());
                                 return;
                             }

                             if (_req.target().empty() || _req.method() == http::verb::unknown)
                             {
                                 onError("Invalid request");
                                 return;
                             }

                             self->handleRequest();
                         });
    }

    /// @brief accept connections from Telegram bot
    /// @param none
    /// @return none
    void SimpleHttpServer::Session::acceptConnections()
    {
        _acceptor.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    auto new_session = std::make_shared<Session>(std::move(socket), _sipeto, _acceptor);
                    new_session->start();
                }
                acceptConnections();
            });
    }

    /// @brief handle incoming requests from Telegram bot
    /// @param none
    /// @return none
    void SimpleHttpServer::Session::handleRequest()
    {
        spdlog::info("Handling request...");

        // Check if the incoming request is a Telegram bot update
        if (_req.target() == "/" + _sipeto.getFromConfigMap("token"))
        {
            spdlog::info("Received Telegram bot update");
            // Process Telegram bot update
            processTelegramUpdate();
        }
        else
        {
            spdlog::info("Received HTTP request");
            // Process request using Sipeto class (existing implementation)
            std::string result = _sipeto.processRequest(_req.body());

            // Generate the HTTP response (existing implementation)
            http::response<http::string_body> res{http::status::ok, _req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/plain");
            res.keep_alive(_req.keep_alive());
            res.body() = result;
            res.prepare_payload();
            writeResponse(res);
        }
    }

    /// @brief write response to the client
    /// @param bad_request_res HTTP response with status 400 Bad Request
    /// @return none
    void SimpleHttpServer::Session::writeResponse(const http::response<http::string_body> &response)
    {
        auto self = shared_from_this();
        http::async_write(_socket, response,
                          [self, response](boost::system::error_code ec, std::size_t)
                          {
                              if (ec)
                              {
                                  spdlog::error("Error writing response: {}", ec.message());
                                  return;
                              }
                              if (self->_res.need_eof())
                              {
                                  boost::system::error_code ec;
                                  self->_socket.shutdown(tcp::socket::shutdown_send, ec);
                                  if (ec)
                                  {
                                      spdlog::error("Error shutting down socket: {}", ec.message());
                                      return;
                                  }
                              }
                              if (response.result() == http::status::bad_request)
                              {
                                  boost::system::error_code ec;
                                  self->_socket.close(ec);
                                  if (ec)
                                  {
                                      spdlog::error("Error closing socket: {}", ec.message());
                                      return;
                                  }
                              }
                          });
    }

    /// @brief process Telegram bot update
    /// @param none
    /// @return none
    void SimpleHttpServer::Session::processTelegramUpdate()
    {
        // Parse the request body as JSON
        Json::CharReaderBuilder builder;
        Json::Value update;
        std::string errors;
        std::istringstream ss(_req.body());
        if (!Json::parseFromStream(builder, ss, &update, &errors))
        {
            // If parsing fails, return an HTTP response with status 400 Bad Request
            http::response<http::string_body> badRequestRes{http::status::bad_request, _req.version()};
            badRequestRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            badRequestRes.keep_alive(_req.keep_alive());
            badRequestRes.prepare_payload();
            writeResponse(badRequestRes);
            return;
        }

        // Pass the JSON data to the Sipeto class for further processing
        _sipeto.processTelegramUpdate(update);

        // Generate an HTTP response with status 200 OK
        http::response<http::string_body> res{http::status::ok, _req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.keep_alive(_req.keep_alive());
        res.prepare_payload();
        writeResponse(res);
    }
}
