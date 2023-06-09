/** 
 * The Sipeto class is designed to facilitate communication between a
 * Telegram bot and the Telegram API. The class provides functions
 * to send messages and receive updates from the API using HTTPS requests.
**/

#ifndef SIPETO_H
#define SIPETO_H

#include "simple_http_server.h"

namespace sipeto
{
    using tcp = boost::asio::ip::tcp;
    namespace http = boost::beast::http;

    /// NOTE: Keep this function as stand alone function
    /// because it is used in a constexpr context.
    /// the `this` pointer within a constexpr member
    /// function is not allowed because `this`
    /// is not a constant expression.
    constexpr std::size_t hashString(const char *str);

    class Sipeto
    {
    public:

        explicit Sipeto(const std::string &configFile = "sipeto_config.json");

        static std::map<std::string, std::string> _configMap;

        void loadConfig();
        void displayInfo();
        void setLogLevel(const std::string &level);
        void processTelegramUpdate(const Json::Value &update);
        void sendMessage(std::string chat_id, std::string text);
        std::string processRequest(const std::string &requestBody);
        std::shared_ptr<spdlog::logger> getLogger() { return _logger; }

        void parseConfig(const Json::Value &root);
        void validateConfigRoot(const Json::Value &root);
        bool isTargetKey(const std::string &value) const;
        void parseArrayConfig(const Json::Value &arrayValue);
        void parseObjectConfig(const Json::Value &objectValue);
        void processConfigValue(const std::string &key, const Json::Value &value);

        // std::map<std::string, std::string> &mapGetter() { return _configMap; }

        // void processTargetKeys(const Json::Value &configValue, const std::string &key);

        const std::vector<std::string> _targetKeys = {"twitter", "tiktok", "instagram", "facebook"};
        const std::string &getFromConfigMap(const std::string &key, const std::map<std::string, std::string> &configMap = _configMap);

        inline void loadConfigMap(const std::string &key, const std::string &value,
                                  std::map<std::string, std::string> &_configMap)
        {
            _configMap[key] = value;
        }

    private:
        struct User
        {
            int id;
            std::string firsName;
            std::string lastName;
            std::string userName;
        };

        struct Chat
        {
            int id;
            std::string type;
            std::string title;
            std::string userName;
            std::string firstName;
            std::string lastName;
        };

        struct Message
        {
            int messageId;
            User from;
            Chat chat;
            int date;
            std::string text;
        };

        struct Update
        {
            int updateId;
            Message message;
        };

        std::string encodeUrl(std::string str);
        void processUpdate(const Update &update);
        std::vector<Update> parseUpdatesJson(Json::Value root);
        std::string makeRequest(std::string &url, std::string data = "");
        void handleRequest(http::request<http::string_body> &&req, tcp::socket &socket);
        void processTargetKeys(const Json::Value &configValue, const std::string &key);
        // void Sipeto::handleWebhookRequest(const HttpRequest &request, HttpResponse &response);
        // Hash function for use with switch statement in setLogLevel()

        static constexpr std::size_t hash(const char *s, std::size_t h = 0)
        {
            return (*s == '\0') ? h : hash(s + 1, (h * 31) + static_cast<unsigned char>(*s));
        }

        std::string _configFile;
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
