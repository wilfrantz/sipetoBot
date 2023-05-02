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

        std::string getVideoMetadata(const std::string &itemId,
                                     const std::string &bearerToken);

        std::string performHttpGetRequest(const std::string &itemId,
                                          const std::string &bearerToken = " ") override;

        static size_t writeCallback(void *ptr, size_t size, size_t nmemb,
                                    void *userdata);

        ReturnCode downloadMedia() override;

        ~TikTok();

    private:
        Sipeto &_sipeto;
        std::string mediaType = "Unknown media type";
        static std::shared_ptr<spdlog::logger> _logger;
        std::map <std::string, std::string> _attributes;
        std::string MEDIA_URL = "<insert_media_file_url_here>";
    };
}
#endif // TIKTOK_H