#include "include/media_downloader.h"

namespace mediaDownloader
{
    std::shared_ptr<spdlog::logger> MediaDownloader::_logger = spdlog::stdout_color_mt("MediaDownloader");

    MediaDownloader::MediaDownloader()
    {

        _logger = spdlog::get("MediaDownloader");
        if (!_logger)
        {
            _logger = spdlog::stdout_color_mt("MediaDownloader");
        }

        _logger->debug("MediaDownloader constructor");
    }

    MediaDownloader::~MediaDownloader()
    {
        _logger->debug("MediaDownloader destructor");
    }
} // !namespace mediaDownloader