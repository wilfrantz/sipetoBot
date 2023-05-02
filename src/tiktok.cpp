
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
            _logger->error("Invalid TikTok URL: {}", url);
            return;
        }

        // Extract the username and tweet ID from the matched groups
        std::string videoId; 
        std::string username; 

        for (size_t i = 0; i < match.size(); ++i)
        {
            _logger->debug("Match {}: {}", i, match[i].str());
            videoId = match[2].str();
            username = match[1].str();
        }

        // contruct the url for the API request.
        // const std::string API_URL = _sipeto.getFromConfigMap("metaUrl") + videoId;
        // const std::string response = TikTok::getVideoMetadata(videoId, _sipeto.getFromConfigMap("client_key"));
        const std::string response = TikTok::performHttpGetRequest(videoId, _sipeto.getFromConfigMap("client_key"));

        if (response.empty())
        {
            _logger->error("Failed to get media attributes from TikTok API");
            return;
        }

        // _logger->debug("Response: {}", response);
        // TODO: Parse the JSON response from the TikTok API

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(response, root))
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
                for (const Json::Value &value : error)
                {
                    if (value.isMember("message"))
                    {
                        _logger->error("TikTok API error: {}", value["message"].asString());
                    }
                }
            }
            return;
        }

        // check for the media type.
        if (root.isMember("itemInfo") && root["itemInfo"].isMember("itemStruct"))
        {
            const Json::Value &itemStruct = root["itemInfo"]["itemStruct"];
            if (itemStruct.isMember("video"))
            {
                this->mediaType = "video";
                const Json::Value &video = itemStruct["video"];
                if (video.isMember("videoMeta"))
                {
                    _logger->debug("Video meta: {}", video["videoMeta"].toStyledString());
                    const Json::Value &videoMeta = video["videoMeta"];
                    if (videoMeta.isMember("height") && videoMeta.isMember("width"))
                    {
                        _attributes["height"] = std::to_string(videoMeta["height"].asInt());
                        _attributes["width"] = std::to_string(videoMeta["width"].asInt());
                    }
                }
            }
            else if (itemStruct.isMember("music"))
            {
                this->mediaType = "music";
                _attributes["height"] = "0";
                _attributes["width"] = "0";
            }
            else
            {
                _logger->error("{}", this->mediaType);
                return;
            }
        }
        _logger->info("Media type: {}", this->mediaType);

        // Log the message that the function finished getting media attributes
        _logger->debug("Finished getting media attributes for TikTok URL: {}", url);
    }

    /**
     * Performs an HTTP GET request to the specified URL with the specified bearer token.
     *
     * @param url The URL to send the request to.
     * @param bearerToken The bearer token to include in the Authorization header.
     * @return The response body as a string, or an empty string if an error occurs.
     */
    std::string TikTok::performHttpGetRequest(const std::string &videoId, const std::string &bearerToken)
    {
        CURL *curl;
        CURLcode res;
        std::string response;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl)
        {
            std::string url = _sipeto.getFromConfigMap("metaEndpoint") + videoId;
            struct curl_slist *headers = NULL;

            headers = curl_slist_append(headers, ("Authorization: Bearer " + bearerToken).c_str());
            headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:87.0) Gecko/20100101 Firefox/87.0");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, TikTok::writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                curl_easy_cleanup(curl);
                curl_slist_free_all(headers);
                curl_global_cleanup();
                return "";
            }

            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
        }
        else
        {
            std::cerr << "CURL init failed!" << std::endl;
            curl_global_cleanup();
            return "";
        }

        curl_global_cleanup();
        return response;
    }


    /**
     * Callback function to receive the response body from a libcurl request.
     * @param ptr The received data.
     * @param size The size of each data element.
     * @param nmemb The number of data elements.
     * @param userdata A pointer to the string to store the response body in.
     * @return The number of bytes processed (should be size * nmemb).
     */
    size_t TikTok::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((std::string *)userp)->append((char *)contents, size * nmemb);
        return size * nmemb;
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