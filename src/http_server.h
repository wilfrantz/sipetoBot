
// Include Sipeto class header file
#include "Sipeto.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

namespace simple_http_server
{

    class SimpleHTTPServer
    {
    public:
        // SimpleHTTPServer(){}
        SimpleHTTPServer(const std::string &address, const std::string &port);
        void runSessionMethod(){
            session.acceptConnections();}

        void start();

    private:
        class Session : public std::enable_shared_from_this<Session>
        {
        public:
            // Session() = default;
            explicit Session(tcp::socket socket, sipeto::Sipeto &sipeto);

            void acceptConnections();

        private:
            void handleRequest();

            void readRequest();

            void writeResponse();

            tcp::socket _socket;
            boost::beast::flat_buffer _buffer;
            http::request<http::string_body> _req;
            http::response<http::string_body> _res;
            // Add a reference to the Sipeto object
            sipeto::Sipeto &_sipeto;
        };

        Session session;
        boost::asio::io_context _ioc;
        tcp::acceptor _acceptor;
        // Add a Sipeto object
        sipeto::Sipeto _sipeto;
    };
} // !namespace simple_http_server