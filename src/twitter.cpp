
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
        _logger->debug("Getting media attributes for URL: {}", url);

        std::string json_data = "{'title': 'Example Media', 'author': 'John Smith', 'length': '10:30'}";

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(json_data, root))
        {
            spdlog::error("Failed to parse media attributes JSON data");
            return;
        }

        // Extract the attributes from the JSON data and store them in the map
        if (!root.isObject())
        {
            spdlog::error("Invalid media attributes JSON data format: root is not an object");
            return;
        }

        for (const auto &key : root.getMemberNames())
        {
            const auto &value = root[key];
            if (!value.isString())
            {
                spdlog::warn("Ignoring non-string value for media attribute: {}", key);
                continue;
            }
            _attributes[key] = value.asString();
        }

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