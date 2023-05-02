#ifndef TIKTOK_H
#define TIKTOK_H

#include "sipeto.h"
#include "media_downloader.h"

using namespace sipeto;
using namespace mediaDownloader;

namespace tiktok
{

    class TikTok final : mediaDownloader::MediaDownloader
    {
    public:
        TikTok(const std::string &mediaUrl, Sipeto &sipeto);

        void getMediaAttributes(const std::string &url) override;

        std::string performHttpGetRequest(const std::string &url,
                                          const std::string &bearerToken);

        ReturnCode downloadMedia() override;

        ~TikTok();

    private:
        Sipeto &_sipeto;
        static std::shared_ptr<spdlog::logger> _logger;
        std::string MEDIA_URL = "<insert_media_file_url_here>";
    };
}
#endif // TIKTOK_H