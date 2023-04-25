
#include "include/instagram.h"

using namespace sipeto;
using namespace mediaDownloader;

namespace instagram
{
    std::shared_ptr<spdlog::logger> Instagram::_logger = spdlog::stdout_color_mt("Instagram");

    Instagram::Instagram(const std::string &mediaUrl, Sipeto &sipeto)
        : _sipeto(sipeto), MEDIA_URL(mediaUrl)
    {
        _logger->debug("Instagram constructor");
    }

    MediaDownloader::ReturnCode Instagram::downloadMedia()
    {
        return ReturnCode::Ok;
    }

    Instagram::~Instagram()
    {
    }

}