#include "sipeto.h"

namespace sipeto
{
    std::shared_ptr<spdlog::logger> Sipeto::_logger = spdlog::stdout_color_mt("console");

    // Global variable to hold the received update
    // std::atomic<std::string> receivedUpdate("");

    Sipeto::Sipeto() : _configFile("sipeto_config.json")
    {
        setConfig(); // load the config file in the _config map.

        _logger = spdlog::get(getFromConfigMap("project"));
        if (!_logger)
        {
            _logger = spdlog::stdout_color_mt(getFromConfigMap("project"));
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
        catch (const std::out_of_range &e)
        {
            _logger->error("Error retrieving key: {} from config file: {}", key, e.what());
            return errorString = "Error retrieving " + key + "from config file";
            exit(1);
        }
        return errorString;
    }

    /// @brief  Print a welcome message
    /// @param  none.
    /// @return none.
    void Sipeto::displayGreetings()
    {
        spdlog::info("Welcome to {} {}.",
                     getFromConfigMap("project"),
                     getFromConfigMap("version"));
        spdlog::info("{}", getFromConfigMap("description"));

        _logger->debug("Developed by: {}.", getFromConfigMap("author"));
    }

    /// @brief: Define a function to start the server
    /// Handle a request and produce a reply.
    ///@param address[in] address of the request
    ///@param port[in] port of the request
    void Sipeto::startServer()
    {
        const std::string &port = getFromConfigMap("port");
        const std::string &address = getFromConfigMap("address");
        unsigned int numThreads = std::thread::hardware_concurrency();

        try
        {
            // Create an io_context object with multiple worker threads
            boost::asio::io_context ioc{static_cast<int>(numThreads)};

            // Resolve the endpoint and set reuse_address option
            boost::asio::ip::tcp::resolver resolver(ioc);
            boost::asio::ip::tcp::resolver::query query(address, port, boost::asio::ip::resolver_query_base::flags::v4_mapped | boost::asio::ip::resolver_query_base::flags::numeric_service);
            auto endpoints = resolver.resolve(query);

            // Create a TCP acceptor object
            tcp::acceptor acceptor{ioc, {tcp::v4(), static_cast<unsigned short>(std::atoi(port.c_str()))}};

            // Enable reuse_address option
            int fd = acceptor.native_handle();
            int on = 1;
            if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
            {
                spdlog::info("Error setting socket option: {}", strerror(errno));
            }

            // Start accepting incoming connections
            spdlog::info("[Server started] {}:{}", address, port);
            isServerRunning = true;
            std::vector<std::thread> threadPool;
            threadPool.reserve(numThreads);
            for (size_t i = 0; i < numThreads; ++i)
            {
                threadPool.emplace_back([&ioc, this]
                                        { ioc.run(); });
            }

            // Wait for all threads in the thread pool to finish
            for (auto &thread : threadPool)
            {
                thread.join();
            }
        }
        catch (const std::exception &e)
        {
            spdlog::info("Error starting server: {}", e.what());
            exit(1);
        }
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

    /// @brief:
    void Sipeto::readInput()
    {

        // Create a JSON object representing the message to send
        const char *send_message_json = "{\"@type\":\"sendMessage\", \"chat_id\":123456, \"input_message_content\":{\"@type\":\"inputMessageText\", \"text\":{\"@type\":\"formattedText\", \"text\":\"Hello, Telegram!\"}}}";

        // Send the message using the Telegram API
        // td_json_client_execute(nullptr, send_message_json);

        _logger->debug("Message sent.");
    }

    void Sipeto::processTelegramUpdate(const Json::Value &update)
    {
        /// TODO: Implement logic to handle different types of updates
        /// (e.g., messages, inline queries, etc.)
        // and send appropriate responses using the Telegram Bot API
    }

    /// @brief  set up a webHookUrl for Telegram bot
    /// @param none
    /// @return none
    /* NOTE: Once a webHookUrl has been set up for
     * a Telegram bot, it does not need to be set
     * up again unless there is a change the URL or disable the webHookUrl.
     * TODO: check if the webHookUrl is already set */
    void Sipeto::setWebHookUrl()
    {
        spdlog::info("Setting up webHookUrl...");
        std::string url = getFromConfigMap("endpoint") + getFromConfigMap("token") + "/setwebHookUrl?url=" + getFromConfigMap("webHookUrl") + "&webhook_use_self_signed=true";

        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_responseBuffer);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
            }
            else
            {
                // Check if webHookUrl is already set
                std::string responseString = _responseBuffer.str();
                Json::Value responseJson;
                Json::Reader reader;
                if (!reader.parse(responseString, responseJson))
                {
                    spdlog::error("Failed to parse JSON response from getwebHookUrlInfo.");
                }
                else
                {
                    bool webHookUrlIsSet = responseJson["result"].asBool(); // == _sipeto.getFromConfigMap("webHookUrl");

                    if (webHookUrlIsSet)
                    {
                        spdlog::info("{}.", responseJson["description"].asString());
                        return;
                    }
                }
            }
        }
        else
        {
            spdlog::error("curl_easy_init() failed: {}", curl_easy_strerror(res));
        }
    }

    size_t Sipeto::writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        size_t realsize = size * nmemb;
        std::stringstream *responseBuffer = (std::stringstream *)userdata;
        responseBuffer->write(ptr, realsize);
        return realsize;
    }
} // !namespace sipeto
