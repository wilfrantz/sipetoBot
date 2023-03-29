#include "sipeto.h"
#include "header.h"
#include <filesystem>
#include <boost/json/src.hpp>

namespace sipeto
{

    std::shared_ptr<spdlog::logger> Sipeto::_logger = spdlog::stdout_color_mt("console");

    Sipeto::Sipeto() : _configPath(std::filesystem::current_path().string() + "/config.json"),
                       _uri("https://api.telegram.org/bot:" + _token + "/")
    {
        _logger = spdlog::get("console");
        if (!_logger)
        {
            _logger = spdlog::stdout_color_mt("console");
        }
        // setToken();
    }

    /// @brief Set the token from the config file
    /// @param none.
    /// @return none.
    void Sipeto::setToken()
    {
        std::ifstream file(_configPath);

        if (!file.is_open())
        {
            _logger->error("Could not open config file.");
            exit(1);
        }
        boost::json::error_code ec;
        boost::json::value j = boost::json::parse(file, ec);
        if (ec)
        {
            // handle parse error
        }
        _token = j.at("token").as_string().data();
    }
}