#ifndef MEDIA_DOWNLOADER_H
#define MEDIA_DOWNLOADER_H

#include "header.h"

namespace mediaDownloader
{

    class MediaDownloader
    {
    public:

        enum class ReturnCode
        {
            Ok,
            InvalidMediaUrl,
            ApiRequestError,
            MediaDownloadError,
            MediaAttributesParseError,
        };

        virtual ReturnCode downloadMedia() = 0;
        virtual void getMediaAttributes(const std::string &url) = 0;
        virtual void loadConfigMap(const std::string &key, const std::string &value) = 0;
        const std::string &getFromConfigMap(const std::string &key, const std::map<std::string, std::string> &configMap);

        std::string makeHttpRequest(const std::string &url,
                                    const std::string &method = "GET",
                                    const std::string &data = {},
                                    const std::map<std::string, std::string> &headers = {});

        virtual std::string performHttpGetRequest(const std::string &url,
                                                  const std::string &bearerToken);


        static size_t writeCallback(char *ptr,
                                    size_t size,
                                    size_t nmemb,
                                    std::string *data);


        virtual ~MediaDownloader() = default;

    protected:
        static std::shared_ptr<spdlog::logger> _logger;
        const std::map<std::string, std::string> &configMap;
    };

} // !namespace mediaDownloader

#endif // !MEDIA_DOWNLOADER_H