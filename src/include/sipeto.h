/*The Sipeto class is designed to facilitate communication between a
Telegram bot and the Telegram API. The class provides functions
to send messages and receive updates from the API using HTTPS requests.*/

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

        void loadConfig();
        void displayInfo();
        void setLogLevel(const std::string &level);
        void processTelegramUpdate(const Json::Value &update);
        void sendMessage(std::string chat_id, std::string text);
        std::string processRequest(const std::string &requestBody);
        const std::string &getFromConfigMap(const std::string &key);
        std::shared_ptr<spdlog::logger> getLogger() { return _logger; }


    private:
        struct User
        {
            int id;
            std::string first_name;
            std::string last_name;
            std::string username;
        };

        struct Chat
        {
            int id;
            std::string type;
            std::string title;
            std::string username;
            std::string first_name;
            std::string last_name;
        };

        struct Message
        {
            int message_id;
            User from;
            Chat chat;
            int date;
            std::string text;
        };

        struct Update
        {
            int update_id;
            Message message;
        };

        std::string encodeUrl(std::string str);
        void processUpdate(const Update &update);
        std::vector<Update> parseUpdatesJson(Json::Value root);
        std::string makeRequest(std::string &url, std::string data = "");
        void handleRequest(http::request<http::string_body> &&req, tcp::socket &socket);
        // void Sipeto::handleWebhookRequest(const HttpRequest &request, HttpResponse &response);
        // Hash function for use with switch statement in setLogLevel()
        static constexpr std::size_t hash(const char *s, std::size_t h = 0)
        {
            return (*s == '\0') ? h : hash(s + 1, (h * 31) + static_cast<unsigned char>(*s));
        }

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
