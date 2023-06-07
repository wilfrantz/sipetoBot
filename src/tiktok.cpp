
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

    MediaDownloader::ReturnCode TikTok::downloadMedia()
    {
        CURL *curl;
        CURLcode res;
       // get the media attributes. 
        getMediaAttributes(MEDIA_URL);
        _logger->debug("Downloading media from TikTok URL: {}", _attributes["downloadAddr"]);
        exit(0);

        std::string filePath(_sipeto.getFromConfigMap("tiktokOutputPath", this->mapGetter()) + _attributes["id"] + ".mp4");
        std::ofstream outputFile(filePath, std::ios::binary);

        if (!outputFile)
        {
            _logger->error("Error opening output file: {}", _sipeto.getFromConfigMap("tiktokOutputPath", this->mapGetter()));
            return MediaDownloader::ReturnCode::MediaDownloadError;
        }


        // initialize curl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, _attributes["downloadAddr"].c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFileCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outputFile);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                _logger->error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                outputFile.close();
                return MediaDownloader::ReturnCode::MediaDownloadError;
            }
            curl_easy_cleanup(curl);
        }
        else
        {
            _logger->error("Error initializing curl");
            curl_global_cleanup();
            outputFile.close();
            return MediaDownloader::ReturnCode::MediaDownloadError;
        }

        outputFile.close();
        _logger->debug("Media downloaded successfully.");
        return MediaDownloader::ReturnCode::Ok;
    }

    void TikTok::getMediaAttributes(const std::string &url)
    {
        _logger->debug("Getting media attibutes for {}", url);

        // Define regular expression for TikTok URL
        const std::regex tiktokRegex(R"(https?://(?:www\.)?tiktok\.com/@(\w+)/video/(\d{19}))");

        std::smatch match;
        std::string longFormatedUrl;
        if (!std::regex_search(url, match, tiktokRegex))
        {
            longFormatedUrl = TikTok::performHttpGetRequest(url);

            if (longFormatedUrl.empty())
            {
                _logger->error("Failed to validate the link.");
                return;
            }

            longFormatedUrl = extractTiktokUrl(longFormatedUrl);
            if (!std::regex_search(longFormatedUrl, match, tiktokRegex))
            {
                _logger->error("Invalid TikTok URL: {}", url);
                return;
            }
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
        // const std::string response = TikTok::getVideoMetadata(videoId, _sipeto.getFromConfigMap("client_key"));
        const std::string endpoint = _sipeto.getFromConfigMap("metaEndpoint", this->mapGetter()) + videoId;

        const std::string response = TikTok::performHttpGetRequest(endpoint,
                                                                   _sipeto.getFromConfigMap("client_key", this->mapGetter()));

        if (response.empty())
        {
            _logger->error("Failed to get media attributes from TikTok API");
            return;
        }

        // Parse the JSON response from the TikTok API
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

        if (root.isMember("itemInfo") && root["itemInfo"].isMember("itemStruct"))
        {
            const Json::Value &itemStruct = root["itemInfo"]["itemStruct"];
            std::map<std::string, const Json::Value *> mediaTypes = {{"video", nullptr}, {"image", nullptr}, {"music", nullptr}};

            /// find the media by type: TODO: needs improvement.
            for (auto const &mediaType : mediaTypes)
            {
                if (itemStruct.isMember(mediaType.first))
                {
                    mediaTypes[mediaType.first] = &itemStruct[mediaType.first];
                    this->mediaType = mediaType.first;
                    _logger->debug("Media type: {}", this->mediaType);

                    // retrieve attributes
                    if (mediaTypes[this->mediaType])
                    {
                        const Json::Value &media = *mediaTypes[this->mediaType];
                        for (const auto &key : media.getMemberNames())
                        {
                            if (media[key].isString())
                            {
                                _logger->debug("{} key: {}", this->mediaType, key);
                                _attributes[key] = media[key].asString();
                            }
                        }
                    }
                    else
                    {
                        _logger->error("Failed to get media attributes from TikTok API");
                        return;
                    }
                }
            }
        }
        // Log the message that the function finished getting media attributes
        _logger->debug("Finished getting media attributes for TikTok URL: {}", url);
    }

    std::string TikTok::extractTiktokUrl(const std::string &html)
    {
        std::string url;

        // Define regular expression for TikTok URL
        std::regex tiktokRegex(R"(<a\s+href="https://www.tiktok.com/@\w+/video/\d+)"
                               R"((?:\?\S+)?"\s*>Moved Permanently</a>)");

        // Search for the TikTok URL in the HTML code
        std::smatch match;
        if (std::regex_search(html, match, tiktokRegex))
        {
            // Extract the URL from the matched group
            url = match[0];
            // Remove the <a href=" and ">Moved Permanently</a> parts of the string
            url = url.substr(9, url.length() - 30);
        }

        return url;
    }

    // Function to download the content of a web page using Boost.Asio and SSL
    std::string TikTok::downloadHttpsPage(const std::string &url)
    {
        return "";
    }

    /**
     * Performs an HTTP GET request to the specified URL with the specified bearer token.
     *
     * @param url The URL to send the request to.
     * @param bearerToken The bearer token to include in the Authorization header.
     * @return The response body as a string, or an empty string if an error occurs.
     */
    std::string TikTok::performHttpGetRequest(const std::string &url, const std::string &bearerToken)
    {
        CURL *curl;
        CURLcode res;
        std::string response;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl)
        {
            struct curl_slist *headers = NULL;

            headers = curl_slist_append(headers, ("Authorization: Bearer " + bearerToken).c_str());
            headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:87.0) Gecko/20100101 Firefox/103.0");

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

    size_t TikTok::writeFileCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        std::ofstream *outputFile = reinterpret_cast<std::ofstream *>(userp);
        outputFile->write(reinterpret_cast<char *>(contents), size * nmemb);
        return size * nmemb;
    }

    TikTok::~TikTok()
    {
        _logger->debug("TikTok destructor");
    }
}