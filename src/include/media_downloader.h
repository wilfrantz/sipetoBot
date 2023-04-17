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

        static std::shared_ptr<spdlog::logger> _logger;

        virtual ~MediaDownloader() = 0;

    };

} // !namespace mediaDownloader

#endif // !MEDIA_DOWNLOADER_H