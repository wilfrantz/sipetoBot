
#ifndef SIMPLE_HTTP_SERVER_H
#define SIMPLE_HTTP_SERVER_H

#include "sipeto.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

namespace simpleHttpServer
{
    class SimpleHTTPServer
    {
    public:
        SimpleHTTPServer(const std::string &address = "localhost",
                         const std::string &port = "443");

        void start();
        void setWebhook();
        void runSessionMethod();

        ~SimpleHTTPServer() { curl_global_cleanup(); }

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
            void readRequest();
            void handleRequest();
            void writeResponse();

            tcp::socket _socket;
            sipeto::Sipeto &_sipeto;
            boost::beast::flat_buffer _buffer;
            http::request<http::string_body> _req;
            http::response<http::string_body> _res;
        };

        void createSession();
        std::stringstream _responseBuffer;

        sipeto::Sipeto _sipeto;
        boost::asio::io_context _ioc{1};
        tcp::acceptor _acceptor;
        std::vector<std::shared_ptr<Session>> _sessions;
        static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata);
    };

} // !namespace simpleHttpServer
#endif // !SIMPLE_HTTP_SERVER_H