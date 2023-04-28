
#include "include/instagram.h"

using namespace sipeto;
using namespace mediaDownloader;

namespace instagram
{
    std::shared_ptr<spdlog::logger> Instagram::_logger = spdlog::stdout_color_mt("Instagram");

    Instagram::Instagram(const std::string &mediaUrl, Sipeto &sipeto)
        : _sipeto(sipeto), MEDIA_URL(mediaUrl)
    {
        _logger->debug("Instagram constructor");
    }

    MediaDownloader::ReturnCode Instagram::downloadMedia()
    {
        _logger->debug("Downloading media.");

        // Initialize libcurl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL *curl = curl_easy_init();

        if (curl)
        {
            // Set the URL of the media to be downloaded
            std::string mediaUrl = _sipeto.getFromConfigMap("instagramMediaUrl"); // Video URL
            curl_easy_setopt(curl, CURLOPT_URL, mediaUrl.c_str());

            // Set the callback function to receive the downloaded media data
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, MediaDownloader::writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &downloadedData);

            // Set additional options for video download
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_RANGE, "0-"); // Download entire video, starting from byte 0

            // Perform the request
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                _logger->error("Failed to download media: %s", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return ReturnCode::MediaDownloadError;
            }

            // Save the downloaded media to a file
            std::string filename = "<media-id>.mp4";
            std::ofstream file(filename, std::ios::binary);
            file.write(downloadedData.c_str(), downloadedData.size());
            file.close();

            _logger->debug("Media downloaded successfully.");
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return ReturnCode::Ok;
        }

        _logger->error("Failed to initialize libcurl.");
        curl_global_cleanup();
        return ReturnCode::MediaDownloadError;
    } // !downloadMedia()

    void Instagram::getMediaAttributes(const std::string &url)
    {
        const std::string lowerUrl = boost::algorithm::to_lower_copy(url);
        _logger->debug("Getting media attributes for URL: {}", url);

        // Define regular expression for an Instagram link
        // std::regex instagramRegex(R"(https?://(?:www\.)?instagram\.com/(?:p|reel|tv)/([^\?/]+)(\?.*)?)", std::regex_constants::icase);
        std::regex instagramRegex(R"(https?://(?:www\.)?instagram\.com/p/(\w+)/?)", std::regex_constants::icase);

        // Match the URL against the regular expression
        std::smatch urlMatch;
        if (!std::regex_search(url, urlMatch, instagramRegex))
        {
            _logger->error("Invalid Instagram link: {}", url);
            return;
        }

        // Ensure that the media ID is available in the matched group
        if (urlMatch.size() < 2)
        {
            _logger->error("Could not extract media ID from URL: {}", lowerUrl);
            return;
        }

        const std::string mediaId = urlMatch[1];

        // Log matched groups for debugging purposes
        for (size_t i = 0; i < urlMatch.size(); ++i)
        {
            _logger->debug("Match {}: {}", i, urlMatch[i].str());
        }

        // Construct the URL for the Instagram API endpoint that returns media information
        const std::string apiUrl("https://graph.instagram.com/" + mediaId + "?fields=id,media_type,media_url,thumbnail_url,permalink,timestamp&access_token=" + _sipeto.getFromConfigMap("instagramToken"));


        // Make an HTTP request to the Instagram API to get the media information
        const std::string responseData = MediaDownloader::performHttpGetRequest(apiUrl, "");
        _logger->debug("Response data: {}", responseData);
        exit(0);

        if (responseData.empty())
        {
            _logger->error("Failed to get media attributes from Instagram API");
            return;
        }

        // Parse the JSON response from the Instagram API
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(responseData, root))
        {
            _logger->error("Failed to parse media attributes JSON data");
            return;
        }

        // Extract the media information from the JSON data and store it in the map
        _attributes["type"] = root["type"].asString();
        _attributes["url"] = root["thumbnail_url"].asString();
        _attributes["width"] = std::to_string(root["thumbnail_width"].asInt());
        _attributes["height"] = std::to_string(root["thumbnail_height"].asInt());

        _logger->debug("Finished getting media attributes for URL: {}", url);
    }

    Instagram::~Instagram()
    {
    }

}