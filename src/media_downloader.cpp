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
    }

    MediaDownloader::~MediaDownloader()
    {
        _logger->info("MediaDownloader destructor");
    }
} // !namespace mediaDownloader