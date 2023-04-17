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
        SimpleHttpServer(boost::asio::io_context &ioc,
                         const tcp::endpoint &endpoint,
                         sipeto::Sipeto &sipeto);
        void run();
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
        boost::asio::io_context _ioc;
        class Session : public std::enable_shared_from_this<Session>
        {
        public:
            explicit Session(tcp::socket socket,
                             sipeto::Sipeto &sipeto,
                             tcp::acceptor &acceptor);

            void start();
            void processTelegramUpdate();

        private:
            void readRequest();
            void handleRequest();
            void handleMethodNotAllowed();
            void handleRequestError(const std::string &errorMessage);
            void writeResponse(const http::response<http::string_body> &response);

            void doRead();


            tcp::socket _socket;
            sipeto::Sipeto &_sipeto;
            tcp::acceptor &_acceptor;
            boost::beast::flat_buffer _buffer;
            http::request<http::empty_body> _req;
            http::response<http::string_body> _res;
            http::request<http::string_body> _reqString;
        };

        void doAccept();
        void createSession();
        std::stringstream _responseBuffer;
        std::unique_ptr<tcp::acceptor> _acceptor; 
        std::vector<std::shared_ptr<Session>> _sessions{};
        static size_t writeCallback(char *ptr, size_t size,
                                    size_t nmemb, void *userdata);
    };
} // !namespace SimpleHttpServer
#endif // !SIMPLE_HTTP_SERVER_H
