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
        explicit Sipeto(const std::string &configFile = "sipeto_config.json");

        void startServer();
        void displayProgramInfo();
        void processTelegramUpdate(const Json::Value &update);
        std::string processRequest(const std::string &requestBody);
        const std::string &getFromConfigMap(const std::string &key);

    private:
        void setConfig();
        void handleRequest(http::request<http::string_body> &&req, tcp::socket &socket);

        std::string _configFile;
        std::map<std::string, std::string> _config;
        static std::shared_ptr<spdlog::logger> _logger;

        std::string receivedUpdate;
        bool isServerRunning = false;
        std::mutex receivedUpdateMutex;
#ifdef UNIT_TEST
        friend class SipetoTest;
#endif
    };

} // !namespace sipeto
#endif // !SIPETO_H
