#ifndef TWITTER_H
#define TWITTER_H

/**
 * @file twitter.h
 * @brief This file contains the declaration of the Twitter class.
 * NOTE: Security: Use encrypted communication such as HTTPS to protect
 * sensitive information such as bearer tokens. You can also store bearer
 * tokens in an encrypted file or environment variable instead of passing
 * them as plain text strings.
 */

#include "sipeto.h"
#include "media_downloader.h"

using namespace sipeto;
using namespace mediaDownloader;

namespace twitter
{

    class Twitter final : mediaDownloader::MediaDownloader
    {
    public:
        Twitter(const std::string &mediaUrl, Sipeto &sipeto);

        void getMediaAttributes(const std::string &url);

        ReturnCode downloadMedia() override;

        ~Twitter();

    private:
        Sipeto &_sipeto;
        std::string _bearerToken{"token"};
        const std::string &_mediaId{"id"};
        const std::string &_outputFilePath{"path"};
        std::map<std::string, std::string> _attributes;
        static std::shared_ptr<spdlog::logger> _logger;

        std::string MEDIA_URL = "<insert_media_file_url_here>";
        const std::string API_URL = _sipeto.getFromConfigMap("twitter_api_url");

        static size_t writeCallback(char *ptr, size_t size,
                                    size_t nmemb,
                                    std::string *data);

#ifdef twitterTest
        friend class twitterTest;
#endif // !twitterTest
    };
} // !namespace twitter

#endif // !TWITTER_H