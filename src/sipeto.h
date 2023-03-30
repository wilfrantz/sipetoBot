#ifndef SIPETO_H
#define SIPETO_H

#include "header.h"
#include <filesystem>
#include <td/telegram/td_json_client.h>

namespace sipeto
{
    class Sipeto
    {
    public:
        Sipeto();
        ~Sipeto() = default;
        void setToken();
        void setConfig();
        const std::string &readFromMap(const std::string &key);
        std::shared_ptr<spdlog::logger> getLogger() const { return _logger; }

        void readInput();
        void returnMedia();

    private:
        std::filesystem::path _currentPath{}; //{std::filesystem::current_path().string()};
        std::string _token = "token";
        const std::string _version = "v1.0.0";
        // Queries to Telegram Bot API must be presented in this form:
        // https://api.telegram.org/bot<token>/METHOD_NAME.
        std::string _uri{};
        std::string _author{};
        std::string _configFile {}; //= _path.parent_path().string() + "/config.json";
        std::string _projectName = "Sipeto";
        std::map <std::string, std::string> _config;
        static std::shared_ptr<spdlog::logger> _logger;

#ifdef UNIT_TEST
        friend class SipetoTest;
#endif
    };

} // !namespace sipeto
#endif // SIPETO_H