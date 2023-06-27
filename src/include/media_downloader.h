#ifndef MEDIA_DOWNLOADER_H
#define MEDIA_DOWNLOADER_H

#include "header.h"

namespace mediaDownloader
{

    class MediaDownloader
    {
    public:
        MediaDownloader(){};
        ~MediaDownloader() = default;

        enum class ReturnCode
        {
            Ok,
            InvalidMediaUrl,
            ApiRequestError,
            MediaDownloadError,
            MediaAttributesParseError,
        };

        virtual ReturnCode downloadMedia() = 0;
        virtual std::map<std::string, std::string> &getTheMap() = 0;
        virtual void getMediaAttributes(const std::string &url) = 0;

        virtual void loadConfigMap(const Json::Value &root, const std::string &socialMedia,
                                   std::map<std::string, std::string> &configMap);
        const std::string &getFromConfigMap(const std::string &key,
                                            const std::map<std::string, std::string> &configMap);


        static size_t writeCallback(char *ptr,
                                    size_t size,
                                    size_t nmemb,
                                    std::string *data);

        virtual std::string performHttpGetRequest(const std::string &url,
                                                  const std::string &bearerToken);

        std::string makeHttpRequest(const std::string &url,
                                    const std::string &method = "GET",
                                    const std::string &data = {},
                                    const std::map<std::string, std::string> &headers = {});

        /// TIP: Used to display configuration map for debugging purpose.
        virtual inline void displayMap(std::map<std::string, std::string> &_configMap)
        {
            if (_configMap.empty())
            {
                _logger->error("Config map is empty.");
                exit(1);
            }
            for (auto &i : _configMap)
            {
                _logger->debug("{} : {}", i.first, i.second);
            }
        }

    protected:
        static std::shared_ptr<spdlog::logger> _logger;
        const std::map<std::string, std::string> _configMap;
    };

} // !namespace mediaDownloader

#endif // !MEDIA_DOWNLOADER_H