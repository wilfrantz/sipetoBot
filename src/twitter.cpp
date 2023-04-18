
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
        std::string authHeader = "Authorization: Bearer " + _bearerToken;
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

        // Define the regular expression for a Twitter link
        // static const std::regex twitterRegex("(https?:\\/\\/)?(www\\.)?twitter\\.com\\/([^\\/]+)\\/status\\/([^\\/?]+)");
        const std::regex twitterRegex(R"(https?://)?(www\.)?twitter\.com/([^/]+)/status/([^/?]+))");

        // Convert the URL to lowercase before matching it against the regex
        const std::string lowerUrl = boost::algorithm::to_lower_copy(url);

        _logger->debug("Getting media attributes for URL: {}", lowerUrl);

        // Match the URL against the regular expression
        std::smatch match;
        if (!std::regex_match(lowerUrl, match, twitterRegex))
        {
            _logger->error("Invalid Twitter link: {}", lowerUrl);
            return;
        }

        // Extract the username and tweet ID from the matched groups
        const std::string username = match[3];
        const std::string tweetId = match[4];

        // Construct the URL for the Twitter API endpoint that returns media information
        const std::string apiUrl = "https://api.twitter.com/2/tweets/" + tweetId + "?expansions=attachments.media_keys&media.fields=duration_ms,height,media_key,preview_image_url,type,url,width&tweet.fields=public_metrics&user.fields=public_metrics,username";

        // Make an HTTP request to the Twitter API to get the media information
        const std::string responseData = makeHttpRequest(apiUrl, {}, {}, {{"Authorization", "Bearer " + _bearerToken}});
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

    Twitter::~Twitter()
    {
        _logger->debug("Twitter destructor.");
    } // !Twitter::~Twitter

} // !namespace twitter