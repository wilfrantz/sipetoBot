#include "sipeto.h"
#include "simple_http_server.h"

namespace simpleHttpServer
{
    /// @brief simple http server constructor
    /// @param ioc
    /// @param endpoint
    SimpleHttpServer::SimpleHttpServer(boost::asio::io_context &ioc, const tcp::endpoint &endpoint, sipeto::Sipeto &sipeto)
        : _sipeto(sipeto), _acceptor(std::make_unique<tcp::acceptor>(ioc))
    {
        boost::system::error_code ec;

        // Open the acceptor
        _acceptor->open(endpoint.protocol(), ec);
        if (ec)
        {
            SPDLOG_LOGGER_ERROR(_sipeto.getLogger(), "Failed to open the acceptor: {}", ec.message());
            return;
        }

        // Allow address reuse
        _acceptor->set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec)
        {
            SPDLOG_LOGGER_ERROR(_sipeto.getLogger(), "Failed to set acceptor option: {}", ec.message());
            return;
        }

        // Bind to the server address
        _acceptor->bind(endpoint, ec);
        if (ec)
        {
            SPDLOG_LOGGER_ERROR(_sipeto.getLogger(), "Failed to bind the acceptor: {}", ec.message());
            return;
        }

        // Start listening for connections
        _acceptor->listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec)
        {
            SPDLOG_LOGGER_ERROR(_sipeto.getLogger(), "Failed to listen on the acceptor: {}", ec.message());
            return;
        }
    }

    /// @brief run the http server
    /// @param none
    /// @return none
    void SimpleHttpServer::run()
    {
        spdlog::info("Running the Server.");
        if (!_acceptor || !_acceptor->is_open())
        {
            SPDLOG_LOGGER_ERROR(_sipeto.getLogger(), "Acceptor is not open");
            return;
        }

        setwebHookUrl();
        createSession();
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard(_ioc.get_executor());
        _ioc.run();
        // createSession();
    }

    /// @brief start the http server
    /// @param none
    /// @return none
    void SimpleHttpServer::start()
    {
        spdlog::info("Start Http Server.");

        setwebHookUrl();
        createSession();

        // Keep the io_context alive to prevent the server from stopping prematurely
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard(_ioc.get_executor());

        // Run the io_context in a separate thread
        std::thread([&]
                    { _ioc.run(); })
            .detach();
    }

    void SimpleHttpServer::doAccept()
    {
        _acceptor->async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket), _sipeto, *_acceptor)->start();
                }
                doAccept();
            });
    }

    /// @brief create a session
    /// @param none
    /// @return none
    void SimpleHttpServer::createSession()
    {
        spdlog::info("Creating Session...");
        SPDLOG_LOGGER_DEBUG(_sipeto.getLogger(), "Creating session...");

        _acceptor->async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    spdlog::info("Session accepted.");
                    SPDLOG_LOGGER_DEBUG(_sipeto.getLogger(), "Session accepted.");
                    auto session = std::make_shared<Session>(std::move(socket), _sipeto, *_acceptor);
                    _sessions.push_back(session);
                    SPDLOG_LOGGER_DEBUG(_sipeto.getLogger(), "Session created.");
                    session->start();
                }
                else
                {
                    spdlog::info("Failed to accept session: {}", ec.message());
                    spdlog::error("Failed to accept session: {}", ec.message());
                    SPDLOG_LOGGER_ERROR(_sipeto.getLogger(), "Failed to accept session: {}", ec.message());
                }

                createSession();
            });

        SPDLOG_LOGGER_DEBUG(_sipeto.getLogger(), "createSession method completed.");
        _sipeto.getLogger()->debug("CreateSession method completed.");
    }

    /// @brief write callback function for curl
    /// @param ptr
    /// @param size
    /// @param nmemb
    /// @param userdata
    /// @return size_t
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
        : _socket(std::move(socket)),
          _sipeto(sipeto),
          _acceptor(acceptor) {}

    /// @brief Start the asynchronous operation
    /// @param none
    /// @return none
    void SimpleHttpServer::Session::start()
    {
        spdlog::info("Starting session...");
        readRequest();
    }

    void SimpleHttpServer::Session::doRead()
    {
        // Read a request
        http::async_read(_socket, _buffer, _req,
                         [this](boost::system::error_code ec, std::size_t)
                         {
                             if (!ec)
                             {
                                 handleRequest();
                             }
                             else if (ec != boost::asio::error::operation_aborted)
                             {
                                 handleRequestError(ec.message());
                             }
                         });
    }

    /// @brief read incoming requests from Telegram bot
    /// @param none
    /// @return none
    void SimpleHttpServer::Session::readRequest()
    {
        SPDLOG_LOGGER_DEBUG(_sipeto.getLogger(), "Reading request...");

        auto self = shared_from_this();

        http::async_read(_socket, _buffer, _req, [self, this](boost::system::error_code ec, std::size_t bytesTransferred)
                         {
        if (ec) {
            handleRequestError("Failed to read request: " + ec.message());
            return;
        }

        // Store the buffer data before it's consumed by the parser
        std::string requestString(static_cast<const char*>(_buffer.data().data()), _buffer.data().size());

        // Create an HTTP request object and parse the received data into it
        http::request_parser<http::empty_body> parser;
        boost::beast::error_code parseError;
        parser.put(_buffer.data(), parseError);
        if (parseError) {
            handleRequestError("Failed to parse request: " + parseError.message() + ", request string: " + requestString);
            return;
        }
        _req = parser.release();

        if (_req.target().empty() || _req.method() == http::verb::unknown) {
            handleRequestError("Invalid request: missing target or unknown method");
            return;
        }

        switch (_req.method()) {
            case http::verb::get:
                self->handleRequest();
                break;
            default:
                handleMethodNotAllowed();
                break;
        }

        SPDLOG_LOGGER_DEBUG(_sipeto.getLogger(), "Read {} bytes of request data", bytesTransferred);

        // If the entire request data has not been read, continue reading asynchronously
        if (_req.need_eof()) {
            return;
        } else {
            readRequest();
        } });
    }

    /// @brief handle errors in incoming requests from Telegram bot.
    /// @param error message
    /// @return none
    void SimpleHttpServer::Session::handleRequestError(const std::string &errorMessage)
    {
        SPDLOG_LOGGER_ERROR(_sipeto.getLogger(), errorMessage);

        // Generate an HTTP response with status 400 Bad Request
        http::response<http::string_body> badRequestRes{http::status::bad_request, _req.version()};
        badRequestRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        badRequestRes.keep_alive(_req.keep_alive());
        badRequestRes.prepare_payload();
        writeResponse(badRequestRes);
    }

    void SimpleHttpServer::Session::handleMethodNotAllowed()
    {
        // Generate an HTTP response with status 405 Method Not Allowed
        http::response<http::string_body> methodNotAllowedRes{http::status::method_not_allowed, _req.version()};
        methodNotAllowedRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        methodNotAllowedRes.keep_alive(_req.keep_alive());
        methodNotAllowedRes.prepare_payload();
        writeResponse(methodNotAllowedRes);
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
            std::string result = _sipeto.processRequest(_reqString.body());

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
        std::istringstream ss(_reqString.body());
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
