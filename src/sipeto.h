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
        const std::string &getAuthor() const { return _author; }
        const std::string &getVersion() const { return _version; }
        std::shared_ptr<spdlog::logger> getLogger() const { return _logger; }

        void readInput();
        void returnMedia();

    private:
        std::string _path{};
        std::string _token = "token";
        const std::string _version = "v1.0.0";
        // Queries to Telegram Bot API must be presented in this form:
        // https://api.telegram.org/bot<token>/METHOD_NAME.
        std::string _configPath{};
        Update _update = getUpdates();
        const std::string _author = "Wilfrantz Dede";
        const std::string _uri = "https://api.telegram.org/bot" + _token + "/";
        static std::shared_ptr<spdlog::logger> _logger;

#ifdef UNIT_TEST
        friend class SipetoTest;
#endif
    };

}
#endif // SIPETO_H