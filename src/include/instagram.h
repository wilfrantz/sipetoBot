#ifndef INSTAGRAM_H
#define INSTAGRAM_H

/**
 * @file instagram.h
 * @brief This file contains the declaration of the Instagram class.
 * 
 * NOTE: Security: Use encrypted communication such as HTTPS to protect
 * sensitive information such as bearer tokens. You can also store bearer
 * tokens in an encrypted file or environment variable instead of passing
 * them as plain text strings.
 */

#include "sipeto.h"
#include "media_downloader.h"

using namespace sipeto;
using namespace mediaDownloader;

namespace instagram
{
    class Instagram final : mediaDownloader::MediaDownloader
    {
    public:
        Instagram(const std::string &mediaUrl, Sipeto &sipeto);

        ReturnCode downloadMedia() override;

        ~Instagram();

    private:
        Sipeto &_sipeto;
        static std::shared_ptr<spdlog::logger> _logger;
        std::string MEDIA_URL = "<insert_media_file_url_here>";
    };

} // !namespace instagram

#endif // !INSTAGRAM_H