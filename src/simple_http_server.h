
#ifndef SIMPLE_HTTP_SERVER_H
#define SIMPLE_HTTP_SERVER_H

#include "header.h"
namespace sipeto
{
    class Sipeto;
}
namespace simpleHttpServer
{
    using tcp = boost::asio::ip::tcp;
    namespace http = boost::beast::http;
    class SimpleHttpServer
    {
    public:
        SimpleHttpServer(sipeto::Sipeto &sipeto,
                         const std::string &address = "localhost",
                         const std::string &port = "8080");

        void start();
        void setwebHookUrl();
        void runSessionMethod();
        sipeto::Sipeto &_sipeto;
        void handleSetWebHookUrlResponse();

        ~SimpleHttpServer()
        {
            curl_global_cleanup();
        }

    private:
        class Session : public std::enable_shared_from_this<Session>
        {
        public:
            explicit Session(tcp::socket socket,
                             sipeto::Sipeto &sipeto,
                             tcp::acceptor &acceptor);

            void start();
            void acceptConnections();
            tcp::acceptor &_acceptor;
            void processTelegramUpdate();

        private:
            void readRequest();
            void handleRequest();
            void writeResponse(const http::response<http::string_body> &response);

            tcp::socket _socket;
            sipeto::Sipeto &_sipeto;
            boost::beast::flat_buffer _buffer;
            http::request<http::string_body> _req;
            http::response<http::string_body> _res;
        };

        void createSession();
        std::string _port{"8080"};
        std::string _address{"localhost"};
        boost::asio::io_context _ioc{1};
        std::stringstream _responseBuffer;
        std::unique_ptr<tcp::acceptor> _acceptor;
        std::vector<std::shared_ptr<Session>> _sessions{};
        static size_t writeCallback(char *ptr, size_t size,
                                    size_t nmemb, void *userdata);
    };
} // !namespace SimpleHttpServer
#endif // !SIMPLE_HTTP_SERVER_H