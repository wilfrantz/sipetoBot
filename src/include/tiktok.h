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


        void getMediaAttributes(const std::string &url) override;

        std::string extractTiktokUrl(const std::string &html);

        std::string downloadHttpsPage(const std::string &url);

        std::string getVideoMetadata(const std::string &itemId,
                                     const std::string &bearerToken);

        std::string performHttpGetRequest(const std::string &itemId,
                                          const std::string &bearerToken = " ") override;

        static size_t writeCallback(void *ptr, size_t size,
                                    size_t nmemb, void *userdata);

        static size_t writeFileCallback(void *contents, size_t size,
                                        size_t nmemb, void *userp);

        ReturnCode downloadMedia() override;

        std::map<std::string, std::string> &mapGetter() { return _configMap; }


        inline void loadConfigMap(const std::string &key, const std::string &value, const std::string &logKey) override
        {
            // mapGetter().emplace(key, value);
            _configMap[key] = value;

        }

        inline void loadConfigMap(const Json::Value &root)
        {
            _logger->info("Loading config map for TikTok.");
            for (auto &key : root.getMemberNames())
            {
                // mapGetter().emplace(key, root[key].asString());
                _configMap[key] = root[key].asString();
            }
        }

        ~TikTok();

    private:
        // Sipeto &_sipeto;
        std::string mediaType = "Unknown media type";
        static std::map<std::string, std::string> _configMap;
        static std::shared_ptr<spdlog::logger> _logger;
        std::map<std::string, std::string> _attributes;
        std::string MEDIA_URL = "<insert_media_file_url_here>";
    };
}
#endif // TIKTOK_H