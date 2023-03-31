#include "simple_http_server.h"

namespace simple_http_server
{
    SimpleHTTPServer::SimpleHTTPServer(const std::string &address,
                                       const std::string &port)
        : _acceptor(_ioc, {tcp::v4(), static_cast<unsigned short>(std::stoi(port))}),
          _sipeto() // Initialize Sipeto object
    {
        createSession();
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
        if (_req.target() == "/_bot_token")
        {
            // Process the Telegram bot update
            processTelegramUpdate();
        }
        else
        {
            // Process the request using the Sipeto class (existing implementation)
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
            // If parsing fails, you can return an error response or handle it in a different way
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
