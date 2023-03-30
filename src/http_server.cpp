#include "http_server.h"

namespace simple_http_server
{

    SimpleHTTPServer::SimpleHTTPServer(const std::string &address,
                                       const std::string &port)
        : _acceptor(_ioc, {tcp::v4(), static_cast<unsigned short>(std::stoi(port))}),
          _sipeto() // Initialize Sipeto object
    {
    }

    void SimpleHTTPServer::start()
    {
        // Session session;
        // SimpleHTTPServer session;
        // acceptConnections();
        runSessionMethod();
        _ioc.run();
    }

    SimpleHTTPServer::Session::Session(tcp::socket socket, sipeto::Sipeto &sipeto)
        : _socket(std::move(socket)), _sipeto(sipeto) {}

    void SimpleHTTPServer::Session::acceptConnections()
    {
        _acceptor.async_accept(
            [this](std::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket), _sipeto)->start();
                }
                acceptConnections();
            });
    }

    void SimpleHTTPServer::Session::handleRequest()
    {
        // Process the request using the Sipeto class
        std::string result = _sipeto.processRequest(_req.body());

        // Generate the HTTP response
        http::response<http::string_body> res{http::status::ok, _req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(_req.keep_alive());
        res.body() = result;
        res.prepare_payload();
        writeResponse();
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

} // !namespace simple_http_server