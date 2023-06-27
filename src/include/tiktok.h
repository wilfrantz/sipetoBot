#ifndef TIKTOK_H
#define TIKTOK_H

// #include "sipeto.h"
#include "media_downloader.h"

// using namespace sipeto;
using namespace mediaDownloader;

namespace tiktok
{

    class TikTok final : public MediaDownloader
    {
    public:
        TikTok() = default;

        TikTok(const std::string &mediaUrl)
            : MEDIA_URL(mediaUrl), MediaDownloader()
        {
            _logger->debug("TikTok constructor");
        }

        ReturnCode downloadMedia() override;
        void getMediaAttributes(const std::string &url) override;
        std::string performHttpGetRequest(const std::string &itemId,
                                          const std::string &bearerToken = " ") override;
        inline std::map<std::string, std::string> &getTheMap() override { return _configMap; }

        std::string extractTiktokUrl(const std::string &html);
        std::string downloadHttpsPage(const std::string &url);
        std::string getVideoMetadata(const std::string &itemId,
                                     const std::string &bearerToken);
        static size_t writeCallback(void *ptr, size_t size,
                                    size_t nmemb, void *userdata);
        static size_t writeFileCallback(void *contents, size_t size,
                                        size_t nmemb, void *userp);

        ~TikTok();

    private:
        std::string mediaType = "Unknown media type";
        static std::shared_ptr<spdlog::logger> _logger;
        std::map<std::string, std::string> _attributes;
        static std::map<std::string, std::string> _configMap;
        std::string MEDIA_URL = "<insert_media_file_url_here>";
    };
}
#endif // TIKTOK_H