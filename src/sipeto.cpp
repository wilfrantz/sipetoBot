#include "sipeto.h"

namespace sipeto
{
    std::shared_ptr<spdlog::logger> Sipeto::_logger = spdlog::stdout_color_mt("console");

    Sipeto::Sipeto(const std::string &configFIle) : _configFile(configFIle)
    {

        if (!configFIle.empty())
        {
            setConfig();
        }

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
