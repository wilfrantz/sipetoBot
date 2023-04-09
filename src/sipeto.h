#ifndef SIPETO_H
#define SIPETO_H

// #include "header.h"
#include "simple_http_server.h"

namespace sipeto
{
    using tcp = boost::asio::ip::tcp;
    namespace http = boost::beast::http;
    class Sipeto
    {
    public:
        Sipeto(const std::string &configFile = "sipeto_config.json");

        void setConfig();
        void startServer();
        void displayGreetings();
        void processTelegramUpdate(const Json::Value &update);
        std::string processRequest(const std::string &requestBody);
        const std::string &getFromConfigMap(const std::string &key);
        std::shared_ptr<spdlog::logger> getLogger() const { return _logger; }
        void handleRequest(http::request<http::string_body> &&req, tcp::socket &socket);
        boost::asio::ip::tcp::resolver::results_type resolveEndpoints(boost::asio::io_context &ioc, const std::string &address, const std::string &port);
        tcp::acceptor createAcceptor(boost::asio::io_context &ioc, const boost::asio::ip::tcp::resolver::results_type &endpoints);
        void acceptConnections(boost::asio::io_context &ioc, tcp::acceptor &acceptor, const std::string &address, const std::string &port);

        std::string receivedUpdate{};
        std::mutex receivedUpdateMutex;

        std::mutex updateMutex;
        bool isServerRunning = false;
        bool updateAvailable = false;
        std::stringstream _responseBuffer;
        std::condition_variable updateReceived;

        ~Sipeto() = default;

    private:
        std::mutex _requestMutex;
        std::string _configFile{};
        std::condition_variable _requestComplete;
        std::map<std::string, std::string> _config;
        // simpleHttpServer::SimpleHttpServer _httpServer;
        static std::shared_ptr<spdlog::logger> _logger;

#ifdef UNIT_TEST
        friend class SipetoTest;
#endif
    };

} // !namespace sipeto
#endif // !SIPETO_H
