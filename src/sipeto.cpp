#include "sipeto.h"
#include <filesystem>
#include <boost/json/src.hpp>

namespace sipeto
{
    std::shared_ptr<spdlog::logger> Sipeto::_logger = spdlog::stdout_color_mt("console");

    // Global variable to hold the received update
    // std::atomic<std::string> receivedUpdate("");

    Sipeto::Sipeto() : _configFile("config.json")
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

    // Update the handle_request function
    void Sipeto::handleRequest(http::request<http::string_body> &&req, tcp::socket &socket)
    {
        // Extract the update from the request body and update the receivedUpdate variable
        {
            std::unique_lock<std::mutex> lock(receivedUpdateMutex);
            receivedUpdate = req.body();
        }

        // ... (rest of the function)
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

    /// @brief: Define a function to start the server
    /// Handle a request and produce a reply.
    ///@param address[in] address of the request
    ///@param port[in] port of the request
    void Sipeto::startServer(const std::string &address, const std::string &port)
    {
        try
        {
            // Create an io_context object
            boost::asio::io_context ioc{1};

            // Create a TCP acceptor object
            tcp::acceptor acceptor{ioc, {tcp::v4(), static_cast<unsigned short>(std::atoi(port.c_str()))}};

            // Start accepting incoming connections
            while (true)
            {
                // Create a TCP socket object
                tcp::socket socket{ioc};

                // Accept a connection
                acceptor.accept(socket);

                // Create a new thread to handle the request
                std::thread{[this](tcp::socket &socket)
                            {
                                http::request<http::string_body> req;
                                this->handleRequest(std::move(req), socket);
                            },
                            std::ref(socket)}
                    .detach();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error starting server: " << e.what() << std::endl;
        }
    }

    std::string Sipeto::processRequest(const std::string &requestBody)
    {
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
        spdlog::info("Welcome to {} {}.",
                     readFromMap("project"),
                     readFromMap("version"));

        spdlog::info("Telegram Social Media Downloader Bot.");
        _logger->debug("Developed by: {}.", readFromMap("author"));

        // Create a JSON object representing the message to send
        const char *send_message_json = "{\"@type\":\"sendMessage\", \"chat_id\":123456, \"input_message_content\":{\"@type\":\"inputMessageText\", \"text\":{\"@type\":\"formattedText\", \"text\":\"Hello, Telegram!\"}}}";

        // Send the message using the Telegram API
        td_json_client_execute(nullptr, send_message_json);

        _logger->debug("Message sent.");
    }
} // !namespace sipeto