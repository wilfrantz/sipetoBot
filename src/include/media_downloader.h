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
            ApiRequestError,
            MediaDownloadError,
            MediaAttributesParseError
        };

        virtual ReturnCode downloadMedia() = 0;
        virtual std::string makeHttpRequest(const std::string &url,
                                    const std::string &method = "GET",
                                    const std::string &data = {},
                                    const std::map<std::string, std::string> &headers = {});

        static std::shared_ptr<spdlog::logger> _logger;

        virtual ~MediaDownloader() = 0;

    };

} // !namespace mediaDownloader

#endif // !MEDIA_DOWNLOADER_H