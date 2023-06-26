// #include "include/sipeto.h"
#include "include/tiktok.h"
#include "include/twitter.h"
#include "include/instagram.h"

namespace sipeto
{
    std::map<std::string, std::string> Sipeto::_configMap;
    std::shared_ptr<spdlog::logger> Sipeto::_logger = spdlog::stdout_color_mt("Sipeto");

    Sipeto::Sipeto(const std::string &configFIle) : _configFile(configFIle)
    {
        _logger = spdlog::get("Sipeto");
        if (!_logger)
        {
            _logger = spdlog::stdout_color_mt("Sipeto");
        }
    }

    /// @brief Set the config file.
    /// @param none.
    /// @return none.
    void Sipeto::loadConfig()
    {
        if (_configFile.empty())
        {
            _logger->error("Configuration file is empty.");
            throw std::runtime_error("Configuration file is empty.");
        }

        try
        {
            _logger->info("Loading configuration file: {}.", _configFile);

            std::ifstream file(_configFile);

            if (!file.is_open())
            {
                throw std::runtime_error("Could not open config file: " + _configFile);
            }

            Json::Value root;
            file >> root;

            validateConfigRoot(root);

            parseConfig(root);
        }
        catch (const std::exception &e)
        {
            _logger->error("Error reading configuration file: {}", e.what());
            throw;
        }
    }

    /// @brief Validate the config file.
    /// @param root Json object.
    /// @return none.
    void Sipeto::validateConfigRoot(const Json::Value &root)
    {
        _logger->info("Validating configuration file.");
        if (!root.isArray())
        {
            throw std::runtime_error("Config file is not an array.");
        }
    }

    /// @brief Parse the config file.
    /// @param root Json object.
    /// @return none.
    void Sipeto::parseConfig(const Json::Value &root)
    {
        _logger->info("Parsing configuration file.");

        for (const auto &object : root)
        {
            if (!object.isObject())
            {
                throw std::runtime_error("Invalid format for object in configuration file.");
            }
            for (const auto &key : object.getMemberNames())
            {
                const auto &value = object[key];
                _logger->debug("Key: {}", key);

                if (isTargetKey(key))
                {
                    processTargetKeys(value, key);
                }
                else if (value.isArray())
                {
                    parseArrayConfig(value);
                }
                else if (value.isObject())
                {
                    parseObjectConfig(value);
                }
                else
                {
                    throw std::runtime_error("Invalid format for object value in configuration file.");
                }
            }
        }
    }

    /// @brief Parse the array config.
    /// @param arrayValue Json object.
    /// @return none.
    void Sipeto::parseArrayConfig(const Json::Value &arrayValue)
    {
        for (const auto &object : arrayValue)
        {
            if (!object.isObject())
            {
                throw std::runtime_error("Invalid format for object in configuration file.");
            }

            parseObjectConfig(object);
        }
    }

    /// @brief  Parse the object config.
    /// @param objectValue Json object.
    /// @return none.
    void Sipeto::parseObjectConfig(const Json::Value &objectValue)
    {
        for (const auto &key : objectValue.getMemberNames())
        {
            const auto &value = objectValue[key];
            processConfigValue(key, value);
        }
    }

    /// @brief  Check if the value is a target key.
    /// @param  value Json object.
    /// @return true if the value is a target key, false otherwise.
    bool Sipeto::isTargetKey(const std::string &key) const
    {
        return std::find(_targetKeys.begin(), _targetKeys.end(), key) != _targetKeys.end();
    }

    /// @brief Process the config value.
    /// @param key 
    /// @param value
    /// @return none. 
    void Sipeto::processConfigValue(const std::string &key, const Json::Value &value)
    {
        if (value.isString())
        {
            _configMap.emplace(key, value.asString());
        }
        else if (value.isInt())
        {
            _configMap.emplace(key, std::to_string(value.asInt()));
        }
        else
        {
            throw std::runtime_error("Invalid format for object value in configuration file.");
        }
    }

    /// @brief Hash a string using the FNV-1a algorithm
    /// @param str[in] The string to hash
    /// @return The hash value of the string
    constexpr std::size_t hashString(const char *str)
    {
        std::size_t hash = 14695981039346656037ULL;
        std::size_t prime = 1099511628211ULL;

        for (std::size_t i = 0; str[i] != '\0'; ++i)
        {
            hash ^= static_cast<std::size_t>(str[i]);
            hash *= prime;
        }

        return hash;
    }

    /// @brief Process the target keys in the config file
    /// @param configValue[in] The value of the target key
    /// @param key[in] The name of the target key
    /// @return none.
    void Sipeto::processTargetKeys(const Json::Value &configValue, const std::string &key)
    {
        if (configValue.isArray())
        {
            for (const auto &element : configValue)
            {
                for (const auto &subKey : element.getMemberNames())
                {
                    const auto &subValue = element[subKey];

                    if (subValue.isString())
                    {
                        // TODO: Add data to the corresponding config map based on the key

                        // Check if the key exists in the map
                        if (_keyMap.count(key) > 0)
                        {
                            const std::string &logKey = _keyMap[key];
                            _logger->info("{}: {} = {}", logKey, subKey, subValue.asString());
                        }
                        else
                        {
                            _logger->info("Unknown key: {} = {}", subKey, subValue.asString());
                        }
                    }
                    else
                    {
                        // Invalid value type
                        spdlog::error("Invalid format for object: {} in the configuration file.", subValue.asString());
                        throw std::runtime_error("Invalid format for object in the configuration file.");
                    }
                }
            }
        }
        else
        {
            // Invalid value type
            throw std::runtime_error("Invalid format for object value in the configuration file.");
        }
    }

    /// @brief Read from the config file
    /// @param key[in] The key to read from the config file.
    /// @return The value of the key or an error string.
    const std::string &Sipeto::getFromConfigMap(const std::string &key,
                                                const std::map<std::string, std::string> &configMap)
    {
        static std::string errorString;

        try
        {
            return configMap.at(key);
        }
        catch (const std::out_of_range &)
        {
            _logger->error("Error retrieving key: {} from config file.", key);
            static const std::string errorString = "Error retrieving " + key + " from config file";
            return errorString;
            exit(1);
        }
        return errorString;
    }

    /// @brief  Print a welcome message
    /// @param  none.
    /// @return none.
    void Sipeto::displayInfo()
    {
        _logger->info("Welcome to {} {}.",
                      getFromConfigMap("project", this->_configMap),
                      getFromConfigMap("project", this->_configMap));
        _logger->info("{}", getFromConfigMap("description", this->_configMap));
        _logger->info("Developed by: {}.", getFromConfigMap("author", this->_configMap));

// Get the Boost version, if available
#ifdef BOOST_LIB_VERSION
        _logger->debug("Using Boost Version: {}.", BOOST_LIB_VERSION);
#else
        _logger->warn("Could not determine Boost version.");
#endif

// Get the jsoncpp version, if available
#ifdef JSONCPP_VERSION_STRING
        _logger->debug("Using jsoncpp Version: {}.", JSONCPP_VERSION_STRING);
#else
        _logger->warn("Could not determine jsoncpp version.");
#endif

// Get the LibCurl version, if available
#ifdef LIBCURL_VERSION
        _logger->debug("Using LibCurl Version: {}.", LIBCURL_VERSION);
#else
        _logger->warn("Could not determine LibCurl version.");
#endif

// Get the spdlog version, if available
#ifdef SPDLOG_VER_MAJOR
        _logger->debug("Using spdlog Version: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
#else
        _logger->warn("Could not determine spdlog version.");
#endif

// Get the OpenSSL version, if available
#ifdef OPENSSL_VERSION_TEXT
        _logger->debug("Using OpenSSL Version: {}.", OPENSSL_VERSION_TEXT);
#else
        _logger->warn("Could not determine OpenSSL version.");
#endif

        // Display the configuration file in debug mode.
        for (const auto &element : this->_configMap)
        {
            _logger->debug("Config: {} = {}", element.first, element.second);
        }
    }

    /// @brief Set the log level.
    /// @param level
    /// return none.
    void Sipeto::setLogLevel(const std::string &level)
    {
        _logger->debug("Setting log level to: {}", level);
        spdlog::level::level_enum log_level;

        switch (hash(level.c_str()))
        {
        case hash("debug"):
            log_level = spdlog::level::debug;
            break;
        case hash("info"):
            log_level = spdlog::level::info;
            break;
        case hash("warn"):
            log_level = spdlog::level::warn;
            break;
        case hash("error"):
            log_level = spdlog::level::err;
            break;
        case hash("critical"):
            log_level = spdlog::level::critical;
            break;
        case hash("off"):
            log_level = spdlog::level::off;
            break;
        default:
            spdlog::warn("Invalid log level '{}'", level);
            return;
        }

        spdlog::set_level(log_level);
        spdlog::stdout_color_mt("sipeto");

        _logger->info("Log level set to: {}", level);
    }

    void Sipeto::sendMessage(std::string chat_id, std::string text)
    {
        // Send the message using the sendMessage method of the Telegram API
        std::string url = getFromConfigMap("endpoint");
        url += getFromConfigMap("token");
        url += "/sendMessage";

        std::string data = "chat_id=" + encodeUrl(chat_id) + "&text=" + encodeUrl(text);
        makeRequest(url, data);
    }

    std::string Sipeto::encodeUrl(std::string str)
    {
        std::string encoded_str = "";
        char c;
        int ic;
        const char *chars = str.c_str();
        char bufHex[10];
        int len = strlen(chars);

        for (int i = 0; i < len; i++)
        {
            c = chars[i];
            ic = c;
            if (c == ' ')
            {
                encoded_str += '+';
            }
            else if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            {
                encoded_str += c;
            }
            else
            {
                sprintf(bufHex, "%X", c);
                if (ic < 16)
                {
                    encoded_str += "%0";
                }
                else
                {
                    encoded_str += "%";
                }
                encoded_str += bufHex;
            }
        }

        return encoded_str;
    }

    std::string Sipeto::makeRequest(std::string &url, std::string data)
    {
        CURL *curl;
        CURLcode res;
        std::string response_string;

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            if (!data.empty())
            {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            }
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
                             {
            ((std::string*)userdata)->append(ptr, size * nmemb);
            return size * nmemb; });
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }

        return response_string;
    }

    void Sipeto::processUpdate(const Update &update)
    {
        // Log the update message for debugging purposes
        SPDLOG_DEBUG("Received update: {}", Json::FastWriter().write(update));

        // Extract the relevant fields from the update message
        std::string chatId = std::to_string(update.message.chat.id);
        std::string userId = std::to_string(update.message.from.id);
        std::string messageText = update.message.text;

        // Process the update by sending a response message
        std::string responseText = "You said: " + messageText;
        sendMessage(chatId, responseText);
    }

    /// @brief: Define a function to handle the request
    /// @param req[in] request
    /// @param socket[in] socket
    /// @return none.
    void Sipeto::handleRequest(http::request<http::string_body> &&req, tcp::socket &socket)
    {
        spdlog::info("Received request from: {}",
                     socket.remote_endpoint().address().to_string());
        // Extract update from request body and update receivedUpdate variable
        {
            std::unique_lock<std::mutex> lock(receivedUpdateMutex);
            receivedUpdate = req.body();
        }

        /// TODO: ... (rest of the function)
    }

    /// @brief  Process the request body and return the response body
    /// @param requestBody
    /// @return none.
    std::string Sipeto::processRequest(const std::string &requestBody)
    {
        spdlog::info("Processing request: {}", requestBody);
        // Perform any processing you need based on the request body
        // For example, parsing JSON data, interacting with a database, or calling other methods in the Sipeto class

        // For demonstration purposes, let's assume the input is a string, and we want to reverse it
        std::string responseBody = requestBody;
        std::reverse(responseBody.begin(), responseBody.end());

        // Return the processed response body
        return responseBody;
    }

    void Sipeto::processTelegramUpdate(const Json::Value &update)
    {
        /// TODO: Implement logic to handle different types of updates
        /// (e.g., messages, inline queries, etc.)
        /// and send appropriate responses using the Telegram Bot API
        /// TODO: Use switch-case instead of if-else. (map, enum)

        // Extract update type
        const std::string &updateType = update["update_type"].asString();

        // handle different types of updates
        if (updateType == "message")
        {
            // handle message update
        }
        else if (updateType == "callback_query")
        {
            // handle callback query update
        }
        else if (updateType == "inline_query")
        {
            // handle inline query update
        }
        else
        {
            // unknown update type
        }
    }

} // !namespace sipeto
