#ifndef SIPETO_H
#define SIPETO_H

#include "header.h"
#include <filesystem>
//#include <td/telegram/td_json_client.h>

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
        void greetings();
        void returnMedia();
        void processTelegramUpdate(const Json::Value &update);
        std::string processRequest(const std::string &requestBody);
        const std::string &getFromConfigMap(const std::string &key);
        std::shared_ptr<spdlog::logger> getLogger() const { return _logger; }
        void startServer(const std::string &address, const std::string &port);
        void handleRequest(http::request<http::string_body> &&req, tcp::socket &socket);

        std::string receivedUpdate{};
        std::mutex receivedUpdateMutex;

        std::mutex updateMutex;
        bool updateAvailable = false;
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
