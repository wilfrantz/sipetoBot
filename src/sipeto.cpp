#include "sipeto.h"
#include <filesystem>
#include <boost/json/src.hpp>

namespace sipeto
{

    std::shared_ptr<spdlog::logger> Sipeto::_logger = spdlog::stdout_color_mt("console");

    Sipeto::Sipeto() : _configFile("config.json"),
                       _uri("https://api.telegram.org/bot:" + _token + "/")
    {
        setConfig();
        _logger = spdlog::get(readFromMap("project"));
        if (!_logger)
        {
            _logger = spdlog::stdout_color_mt(readFromMap("project"));
        }
    }

    /// @brief Set the config file.
    /// @param none.
    /// @return none.
    void Sipeto::setConfig()
    {
        try
        {
            _logger->debug("Loading configuration file: {}.", _configFile);
            std::ifstream file(_configFile);
            if (!file.is_open())
            {
                throw std::runtime_error("Could not open config file.");
            }

            Json::Value root;
            file >> root;
            if (!root.isArray())
            {
                throw std::runtime_error("Config file is not an array.");
            }

            for (const auto &object : root)
            {
                if (!object.isObject())
                {
                    throw std::runtime_error("Invalid format for object in configuration file.");
                }
                for (auto const &key : object.getMemberNames())
                {
                    const auto &value = object[key];
                    if (value.isArray())
                    {
                        // Process data array values
                        for (const auto &element : value)
                        {
                            for (auto const &key : element.getMemberNames())
                            {
                                const auto &value = element[key];
                                if (value.isString())
                                {
                                    // Add data array to _config map
                                    // spdlog::info("{}: {}", key, value.asString());
                                    _config[key] = value.asString();
                                }
                                else
                                {
                                    // Invalid value type
                                    spdlog::error("Invalid format for object: {} in configuration file.", value.asString());
                                    throw std::runtime_error("Invalid format for object in configuration file.");
                                }
                            }
                        }
                    }
                    else if (value.isString())
                    {
                        // Process string values
                        _config[key] = value.asString();
                    }
                    else
                    {
                        // Invalid value type
                        throw std::runtime_error("Invalid format for object value in configuration file.");
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            _logger->error("Error reading configuration file: {}", e.what());
            // return an error code or throw an exception
        }
    }

    /// @brief Read from the config file
    /// @param key[in] The key to read from the config file.
    /// @return The value of the key or an error string.
    const std::string &Sipeto::readFromMap(const std::string &key)
    {
        static std::string errorString;
        ;
        try
        {
            return _config.at(key);
        }
        catch (const std::out_of_range &e)
        {
            _logger->error("Error retrieving key: {} from config file: {}", key, e.what());
            return errorString = "Error retrieving " + key + "from configuration file";
        }
        return errorString;
    }

    /// @brief Set the token from the config file
    /// @param none.
    /// @return none.
    void Sipeto::setToken()
    {
    }

    /// @brief 
    void Sipeto::readInput()
    {

        spdlog::info("Welcome to {} {}.",
                     readFromMap("project"),
                     readFromMap("version"));

        spdlog::info("Social Media Downloader Bot for Telegram.");
        _logger->debug("Developed by: {}.", readFromMap("author"));

        // Create a JSON object representing the message to send
        const char *send_message_json = "{\"@type\":\"sendMessage\", \"chat_id\":123456, \"input_message_content\":{\"@type\":\"inputMessageText\", \"text\":{\"@type\":\"formattedText\", \"text\":\"Hello, Telegram!\"}}}";

        // Send the message using the Telegram API
        td_json_client_execute(nullptr, send_message_json);

        _logger->debug("Message sent.");
    }
} // !namespace sipeto