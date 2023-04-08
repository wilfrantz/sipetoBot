#ifndef SIPETO_H
#define SIPETO_H

#include "header.h"
#include <filesystem>
// #include <td/telegram/td_json_client.h>

namespace sipeto
{
    using tcp = boost::asio::ip::tcp;
    namespace http = boost::beast::http;
    class Sipeto
    {
    public:
        Sipeto();

        void setConfig();
        void readInput();
        void returnMedia();
        void startServer();
        void displayGreetings();
        void setWebHookUrl();
        void processTelegramUpdate(const Json::Value &update);
        std::string processRequest(const std::string &requestBody);
        const std::string &getFromConfigMap(const std::string &key);
        std::shared_ptr<spdlog::logger> getLogger() const { return _logger; }
        void handleRequest(http::request<http::string_body> &&req, tcp::socket &socket);
        boost::asio::ip::tcp::resolver::results_type resolveEndpoints(boost::asio::io_context &ioc, const std::string &address, const std::string &port);
        tcp::acceptor createAcceptor(boost::asio::io_context &ioc, const boost::asio::ip::tcp::resolver::results_type &endpoints);
        void acceptConnections(boost::asio::io_context &ioc, tcp::acceptor &acceptor, const std::string &address, const std::string &port);
        static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata);

        std::string receivedUpdate{};
        std::mutex receivedUpdateMutex;

        std::mutex updateMutex;
        bool isServerRunning = false;
        bool updateAvailable = false;
        std::stringstream _responseBuffer;
        std::condition_variable updateReceived;

        ~Sipeto() = default;

    private:
        std::string _configFile{};
        std::map<std::string, std::string> _config;
        static std::shared_ptr<spdlog::logger> _logger;

#ifdef UNIT_TEST
        friend class SipetoTest;
#endif
    };

} // !namespace sipeto
#endif // !SIPETO_H
