
#include "include/twitter.h"

using namespace sipeto;
using namespace mediaDownloader;

namespace twitter
{
    std::shared_ptr<spdlog::logger> Twitter::_logger = spdlog::stdout_color_mt("Twitter");

    Twitter::Twitter(const std::string &mediaUrl, Sipeto &sipeto)
        : MEDIA_URL(mediaUrl), _sipeto(sipeto)
    {
        _logger->debug("Twitter constructor");
        API_URL = _sipeto.getFromConfigMap("api_url");
    }

    MediaDownloader::ReturnCode Twitter::downloadMedia()
    {
        _logger->debug("Downloading media.");

        // Initialize libcurl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL *curl = curl_easy_init();

        // Set up the API request
        std::string apiUrl = API_URL + _mediaId;
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        std::string authHeader = "Authorization: Bearer " + _sipeto.getFromConfigMap("bearer_token");
        curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the API request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Error performing API request: " << curl_easy_strerror(res) << std::endl;
            return ReturnCode::ApiRequestError;
        }

        /// TODO: Parse the JSON response
        // ...

        // Download the media file
        curl_easy_setopt(curl, CURLOPT_URL, MEDIA_URL.c_str());
        FILE *fp = fopen(_outputFilePath.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        fclose(fp);
        if (res != CURLE_OK)
        {
            _logger->error("Error downloading media file: {}", curl_easy_strerror(res));
            return ReturnCode::MediaDownloadError;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        _logger->debug("Finished downloading media file");
        return ReturnCode::Ok;
    }

    void Twitter::getMediaAttributes(const std::string &url)
    {
        const std::string lowerUrl = boost::algorithm::to_lower_copy(url);
        _logger->debug("Getting media attributes for URL: {}", lowerUrl);

        // define regular expression for a twitter link
        const std::regex twitterRegex(R"(https?://(?:www\.)?twitter\.com/([^/]+)/status/(\d+)(?:\?.*)?)");

        // Match the URL against the regular expression
        std::smatch match;
        if (!std::regex_match(lowerUrl, match, twitterRegex))
        {
            _logger->error("Invalid Twitter link: {}", lowerUrl);
            return;
        }

        // Extract the username and tweet ID from the matched groups, if available
        if (!match.size() >= 3)
        {
            _logger->error("Could not extract username and tweet ID from URL: {}", lowerUrl);
            return;
        }

        for (size_t i = 0; i < match.size(); ++i)
        {
            _logger->debug("Match {}: {}", i, match[i].str());
        }
        const std::string username = match[1];
        const std::string tweetId = match[2];

        // Construct the URL for the Twitter API endpoint that returns media information
        const std::string &apiUrl(_sipeto.getFromConfigMap("api_url") + tweetId + "&expansions=public_metrics&media.fields=preview_image_url,public_metrics");

        // Make an HTTP request to the Twitter API to get the media information
        // const std::string responseData = makeHttpRequest(apiUrl, {}, {}, {{"Authorization", "Bearer " + _sipeto.getFromConfigMap("bearer_token")}});
        const std::string responseData = performHttpGetRequest(apiUrl, _sipeto.getFromConfigMap("bearer_token"));

        if (responseData.empty())
        {
            _logger->error("Failed to get media attributes from Twitter API");
            return;
        }

        // Parse the JSON response from the Twitter API
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(responseData, root))
        {
            _logger->error("Failed to parse media attributes JSON data");
            return;
        }

        // check for errors in the JSON response.
        if (root.isMember("errors"))
        {
            const Json::Value &error = root["errors"];
            if (error.isArray())
            {
                for (const auto &e : error)
                {
                    _logger->error("Error : {}", e["message"].asString());
                }
            }
            else
            {
                _logger->error("Error : {}", error["message"].asString());
            }
            return;
        }

        // Extract the media information from the JSON data and store it in the map
        const Json::Value &media = root["includes"]["media"][0];
        if (media.empty())
        {
            _logger->error("Failed to get media attributes from Twitter API");
            return;
        }

        _attributes["type"] = media["type"].asString();
        _attributes["url"] = media["url"].asString();

        if (media["type"] == "video")
        {
            _attributes["duration"] = std::to_string(media["duration_ms"].asInt() / 1000);
        }
        else
        {
            _attributes["duration"] = "0";
        }

        _attributes["width"] = std::to_string(media["width"].asInt());
        _attributes["height"] = std::to_string(media["height"].asInt());
        _attributes["preview_url"] = media["preview_image_url"].asString();

        _attributes["username"] = root["data"]["author_id"].asString();
        _attributes["tweet_id"] = root["data"]["id"].asString();

        _logger->debug("Finished getting media attributes for URL: {}", url);
    }

    size_t Twitter::writeCallback(char *ptr, size_t size, size_t nmemb, std::string *data)
    {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }

    // Define a function to perform an HTTP GET request using libcurl.
    /// NOTE: it needs to be a member of media downloader class.
    std::string Twitter::performHttpGetRequest(const std::string &url, const std::string &bearerToken)
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

    Twitter::~Twitter()
    {
        _logger->debug("Twitter destructor.");
    } // !Twitter::~Twitter

} // !namespace twitter