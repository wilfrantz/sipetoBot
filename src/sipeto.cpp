#include "include/sipeto.h"

namespace sipeto
{
    std::shared_ptr<spdlog::logger> Sipeto::_logger = spdlog::stdout_color_mt("Sipeto");

    Sipeto::Sipeto(const std::string &configFIle) : _configFile(configFIle)
    {
        // load configuration map.
        loadConfig();

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
            exit(1);
        }

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
                        // Process string values: add data to _config map.
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
    const std::string &Sipeto::getFromConfigMap(const std::string &key)
    {
        static std::string errorString;

        try
        {
            return _config.at(key);
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
                      getFromConfigMap("project"),
                      getFromConfigMap("version"));
        _logger->info("{}", getFromConfigMap("description"));
        _logger->info("Developed by: {}.", getFromConfigMap("author"));

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
        for (const auto &element : _config)
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
        std::string chat_id = std::to_string(update.message.chat.id);
        std::string user_id = std::to_string(update.message.from.id);
        std::string message_text = update.message.text;

        // Process the update by sending a response message
        std::string response_text = "You said: " + message_text;
        sendMessage(chat_id, response_text);
    }
    // void Sipeto::handleWebhookRequest(const HttpRequest &request, HttpResponse &response)
    // {
    //     // Parse the request body as JSON and process each update
    //     Json::Value root;
    //     Json::Reader reader;
    //     reader.parse(request.getBody(), root);
    //     if (root.isObject() && root.isMember("update_id"))
    //     {
    //         Update update;
    //         update.update_id = root["update_id"].asInt();
    //         update.message.message_id = root["message"]["message_id"].asInt();
    //         update.message.date = root["message"]["date"].asInt();
    //         update.message.text = root["message"]["text"].asString();
    //         update.message.from.id = root["message"]["from"]["id"].asInt();
    //         update.message.from.first_name = root["message"]["from"]["first_name"].asString();
    //         update.message.from.last_name = root["message"]["from"]["last_name"].asString();
    //         update.message.from.username = root["message"]["from"]["username"].asString();
    //         update.message.chat.id = root["message"]["chat"]["id"].asInt();
    //         update.message.chat.type = root["message"]["chat"]["type"].asString();
    //         update.message.chat.title = root["message"]["chat"]["title"].asString();
    //         update.message.chat.username = root["message"]["chat"]["username"].asString();
    //         update.message.chat.first_name = root["message"]["chat"]["first_name"].asString();
    //         update.message.chat.last_name = root["message"]["chat"]["last_name"].asString();
    //         processUpdate(update);
    //     }

    //     // Return a 200 OK response to acknowledge receipt of the request
    //     response.setStatus(200);
    // }

    /// @brief: Define a function to start the server
    /// Handle a request and produce a reply.
    ///@param address[in] address of the request
    ///@param port[in] port of the request
    // void Sipeto::startServer()
    // {
    //     const std::string &port = getFromConfigMap("sipeto_port");
    //     const std::string &address = getFromConfigMap("address");

    //     unsigned int numThreads = std::thread::hardware_concurrency();

    //     try
    //     {
    //         // Create an io_context object with multiple worker threads
    //         boost::asio::io_context ioc{static_cast<int>(numThreads)};

    //         // Resolve the endpoint and set reuse_address option
    //         boost::asio::ip::tcp::resolver resolver(ioc);
    //         boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();

    //         // Create a TCP acceptor object and bind it to the endpoint
    //         tcp::acceptor acceptor{ioc, endpoint};

    //         // Enable reuse_address option
    //         acceptor.set_option(boost::asio::socket_base::reuse_address(true));

    //         // Start accepting incoming connections
    //         spdlog::info("Sipeto started [{}:{}]", address, port);
    //         isServerRunning = true;

    //         std::vector<std::thread> threadPool;
    //         threadPool.reserve(numThreads);

    //         // Create a function to accept new connections
    //         auto acceptConnections = [&acceptor, this](boost::asio::io_context &ioc)
    //         {
    //             while (isServerRunning)
    //             {
    //                 tcp::socket socket{ioc};
    //                 acceptor.accept(socket);

    //                 // Process the incoming request in a separate thread
    //                 std::thread t([this, sock = std::move(socket)]() mutable
    //                               {
    //                               http::request<http::string_body> req;
    //                               http::read(sock, buffer, req);
    //                               handleRequest(std::move(req), sock); });
    //                 t.detach();
    //             }
    //         };

    //         // Run the acceptConnections function on each thread in the thread pool
    //         for (size_t i = 0; i < numThreads; ++i)
    //         {
    //             threadPool.emplace_back(acceptConnections, std::ref(ioc));
    //         }

    //         // Wait for all threads in the thread pool to finish
    //         for (auto &thread : threadPool)
    //         {
    //             thread.join();
    //         }
    //     }
    //     catch (const std::exception &e)
    //     {
    //         spdlog::info("Error starting sipeto server: {}", e.what());
    //         exit(1);
    //     }
    // }

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
