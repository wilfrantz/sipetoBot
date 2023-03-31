
#ifndef SIMPLE_HTTP_SERVER_H
#define SIMPLE_HTTP_SERVER_H
// Include Sipeto class header file
#include "sipeto.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

namespace simple_http_server
{

    class SimpleHTTPServer
    {
    public:
        // SimpleHTTPServer() = default;

        void start();
        void runSessionMethod();
        SimpleHTTPServer(const std::string &address, const std::string &port);

        ~SimpleHTTPServer() = default;

    private:
        class Session : public std::enable_shared_from_this<Session>
        {
        public:
            explicit Session(tcp::socket socket,
                             sipeto::Sipeto &sipeto,
                             tcp::acceptor &acceptor);

            void start();
            tcp::acceptor &_acceptor;
            void acceptConnections();
            void processTelegramUpdate();

        private:
            void handleRequest();
            void readRequest();
            void writeResponse();

            tcp::socket _socket;
            sipeto::Sipeto &_sipeto;
            boost::beast::flat_buffer _buffer;
            http::request<http::string_body> _req;
            http::response<http::string_body> _res;
        };

        void createSession();

        sipeto::Sipeto _sipeto;
         boost::asio::io_context _ioc{1};
        tcp::acceptor _acceptor;
        std::vector<std::shared_ptr<Session>> _sessions;
    };

} // !namespace simple_http_server
#endif // !SIMPLE_HTTP_SERVER_H