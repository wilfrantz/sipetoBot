#include "include/media_downloader.h"

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

//     #include <iostream>
// #include <sstream>
// #include <boost/asio.hpp>
// #include <boost/beast.hpp>

// namespace asio = boost::asio;
// namespace beast = boost::beast;
// using tcp = boost::asio::ip::tcp;

std::string MediaDownloader::makeHttpRequest(const std::string& url, const std::string& method = "GET", const std::string& data = {}, const std::map<std::string, std::string>& headers = {}) {
    // Parse the URL to get the hostname and path
    beast::url parsedUrl(url);
    if (!parsedUrl.host() || !parsedUrl.path()) {
        throw std::invalid_argument("Invalid URL: " + url);
    }

    // Resolve the hostname to an IP address
    asio::io_context ioContext;
    tcp::resolver resolver(ioContext);
    tcp::resolver::results_type endpoints = resolver.resolve(parsedUrl.host(), "https");

    // Connect to the server
    beast::ssl_stream<beast::tcp_stream> stream(ioContext, beast::ssl::context(beast::ssl::context::sslv23));
    beast::get_lowest_layer(stream).connect(endpoints);
    stream.handshake(beast::ssl::stream_base::client);

    // Construct the HTTP request message
    beast::http::request<beast::http::string_body> request(beast::http::verb::get, parsedUrl.path(), 11);
    request.set(beast::http::field::host, parsedUrl.host());
    request.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    for (const auto& header : headers) {
        request.set(header.first, header.second);
    }
    if (!data.empty()) {
        request.body() = data;
        request.prepare_payload();
    }

    // Send the HTTP request message and receive the response message
    beast::http::response<beast::http::dynamic_body> response;
    beast::http::write(stream, request);
    beast::http::read(stream, response);

    // Check for an error in the response
    if (response.result() != beast::http::status::ok) {
        throw std::runtime_error("HTTP request failed: " + std::to_string(response.result_int()) + " " + response.reason().to_string());
    }

    // Convert the response message to a string and return it
    std::stringstream ss;
    ss << response;
    return ss.str();
}


    MediaDownloader::~MediaDownloader()
    {
        _logger->debug("MediaDownloader destructor");
    }
} // !namespace mediaDownloader