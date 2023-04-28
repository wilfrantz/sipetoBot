#include "include/media_downloader.h"

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;
namespace mediaDownloader
{
    std::shared_ptr<spdlog::logger> MediaDownloader::_logger = spdlog::stdout_color_mt("MediaDownloader");

    MediaDownloader::MediaDownloader()
    {

        _logger = spdlog::get("MediaDownloader");
        if (!_logger)
        {
            _logger = spdlog::stdout_color_mt("MediaDownloader");
        }

        _logger->debug("MediaDownloader constructor");
    }

#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

    namespace asio = boost::asio;
    using tcp = boost::asio::ip::tcp;

    std::string MediaDownloader::makeHttpRequest(const std::string &url,
                                                 const std::string &method,
                                                 const std::string &data,
                                                 const std::map<std::string, std::string> &headers)
    {
        _logger->debug("Making HTTP request to {}", url);

        // Parse the URL to get the hostname and path
        std::string protocol, hostname, port, path;
        std::istringstream iss(url);
        iss >> protocol >> std::ws >> hostname >> std::ws >> port >> std::ws >> path;

        if (protocol.empty() || hostname.empty() || path.empty())
        {
            throw std::invalid_argument("Invalid URL: " + url);
        }

        // Resolve the hostname to an IP address
        asio::io_context ioContext;
        tcp::resolver resolver(ioContext);
        tcp::resolver::query query(hostname, port);
        tcp::resolver::iterator endpoints = resolver.resolve(query);

        // Connect to the server
        tcp::socket socket(ioContext);
        asio::connect(socket, endpoints);

        // Construct the HTTP request message
        std::ostringstream oss;
        oss << method << " " << path << " HTTP/1.1\r\n";
        oss << "Host: " << hostname << "\r\n";
        oss << "User-Agent: curl/7.61.0\r\n";
        for (const auto &header : headers)
        {
            oss << header.first << ": " << header.second << "\r\n";
        }
        if (!data.empty())
        {
            oss << "Content-Length: " << data.size() << "\r\n";
        }
        oss << "\r\n";
        oss << data;

        // Send the HTTP request message and receive the response message
        std::string request = oss.str();
        asio::write(socket, asio::buffer(request));
        asio::streambuf responseBuffer;
        asio::read_until(socket, responseBuffer, "\r\n\r\n");
        std::stringstream ss;
        ss << &responseBuffer;
        std::string headerString = ss.str();
        std::size_t contentLength = 0;
        std::string::size_type pos = headerString.find("Content-Length: ");
        if (pos != std::string::npos)
        {
            contentLength = std::stoi(headerString.substr(pos + 16));
        }
        ss.str("");
        ss.clear();
        ss << &responseBuffer;
        ss.seekp(headerString.size() + 4);
        while (ss.str().size() < headerString.size() + 4 + contentLength)
        {
            asio::read(socket, responseBuffer, asio::transfer_at_least(1));
            ss << &responseBuffer;
        }
        std::string response = ss.str();

        // Check for an error in the response
        if (response.find("HTTP/1.1 200 OK") != 0)
        {
            throw std::runtime_error("HTTP request failed: " + response.substr(0, response.find("\r\n")));
        }

        // Strip the HTTP header from the response and return the body
        pos = response.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            response = response.substr(pos + 4);
        }

        return response;
    }

    size_t MediaDownloader::writeCallback(char *ptr, size_t size, size_t nmemb, std::string *data)
    {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }

    std::string MediaDownloader::performHttpGetRequest(const std::string &url, const std::string &bearerToken)
    {
        // Initialize the libcurl library
        curl_global_init(CURL_GLOBAL_DEFAULT);

        // Create a new curl session
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            throw std::runtime_error("Failed to initialize curl");
        }

        _logger->debug("performing HTTP GET request to URL: {}", url);

        std::string response;

        // Set the curl session options
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + bearerToken).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            throw std::runtime_error("Failed to perform curl request: " + std::string(curl_easy_strerror(res)));
        }

        // Cleanup and return the HTTP response
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl_global_cleanup();

        _logger->debug("Finished performing HTTP GET request to URL: {}", url);

        return response;
    }

    MediaDownloader::~MediaDownloader()
    {
        _logger->debug("MediaDownloader destructor");
    }
} // !namespace mediaDownloader