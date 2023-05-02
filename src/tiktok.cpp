
#include "include/tiktok.h"

using namespace sipeto;
using namespace mediaDownloader;

namespace tiktok
{
    std::shared_ptr<spdlog::logger> TikTok::_logger = spdlog::stdout_color_mt("TikTok");

    TikTok::TikTok(const std::string &mediaUrl, Sipeto &sipeto)
        : _sipeto(sipeto), MEDIA_URL(mediaUrl)
    {
        _logger->debug("TikTok constructor");
    }

    void TikTok::getMediaAttributes(const std::string &url)
    {
        // Define regular expression for TikTok URL
        const std::regex tiktokRegex(R"(https?://(?:www\.)?tiktok\.com/@(\w+)/video/(\d{19}))");

        std::smatch match;
        if (!std::regex_search(url, match, tiktokRegex))
        {
            // If the URL doesn't match the regex, log the error message and return
            _logger->error("Invalid TikTok URL: {}", url);
            return;
        }

        // Extract the username and tweet ID from the matched groups
        const std::string username = match[1].str();
        const std::string videoId = match[2].str();

        // Log the extracted username and tweet ID
        _logger->debug("TikTok username: {}", username);
        _logger->debug("TikTok video ID: {}", videoId);

        // Initialize libcurl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            // If libcurl fails to initialize, log the error message and return
            _logger->error("Failed to initialize libcurl");
            return;
        }

        // TODO: Add code to extract media attributes using libcurl.
        // This may involve making a request to the TikTok API with the video ID.

        // Clean up libcurl
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        // Log the message that the function finished getting media attributes
        _logger->debug("Finished getting media attributes for TikTok URL: {}", url);
    }

    MediaDownloader::ReturnCode TikTok::downloadMedia()
    {

        return MediaDownloader::ReturnCode::Ok;
    }

    TikTok::~TikTok()
    {
        _logger->debug("TikTok destructor");
    }
}