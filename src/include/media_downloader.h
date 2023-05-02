#ifndef MEDIA_DOWNLOADER_H
#define MEDIA_DOWNLOADER_H

#include "header.h"

namespace mediaDownloader
{

    class MediaDownloader
    {
    public:
        MediaDownloader();

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

        std::string makeHttpRequest(const std::string &url,
                                    const std::string &method = "GET",
                                    const std::string &data = {},
                                    const std::map<std::string, std::string> &headers = {});

        virtual std::string performHttpGetRequest(const std::string &url,
                                                  const std::string &bearerToken);

        static std::shared_ptr<spdlog::logger> _logger;

        static size_t writeCallback(char *ptr,
                                    size_t size,
                                    size_t nmemb,
                                    std::string *data);

        virtual ~MediaDownloader() = 0;

    };

} // !namespace mediaDownloader

#endif // !MEDIA_DOWNLOADER_H