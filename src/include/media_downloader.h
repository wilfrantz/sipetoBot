/**
 * The Social Media Downloader Bot simplifies media downloading from social media links.
 *
 * (C) 2023 DEDE LLC, Wilfrantz Dede
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.
 * If not, see https://github.com/wilfrantz/sipetoBot/blob/main/LICENSE.txt.
 * 
 * File description : 
 * */

#ifndef MEDIA_DOWNLOADER_H
#define MEDIA_DOWNLOADER_H

#include "header.h"

namespace mediaDownloader
{

    class MediaDownloader
    {
    public:
        MediaDownloader(){};
        ~MediaDownloader() = default;

        enum class ReturnCode
        {
            Ok,
            InvalidMediaUrl,
            ApiRequestError,
            MediaDownloadError,
            MediaAttributesParseError,
        };

        virtual ReturnCode downloadMedia() = 0;
        virtual std::map<std::string, std::string> &getTheMap() = 0;
        virtual void getMediaAttributes(const std::string &url) = 0;

        virtual void loadConfigMap(const Json::Value &root, const std::string &socialMedia,
                                   std::map<std::string, std::string> &configMap);
        const std::string &getFromConfigMap(const std::string &key,
                                            const std::map<std::string, std::string> &configMap);


        static size_t writeCallback(char *ptr,
                                    size_t size,
                                    size_t nmemb,
                                    std::string *data);

        virtual std::string performHttpGetRequest(const std::string &url,
                                                  const std::string &bearerToken);

        std::string makeHttpRequest(const std::string &url,
                                    const std::string &method = "GET",
                                    const std::string &data = {},
                                    const std::map<std::string, std::string> &headers = {});

        /// TIP: Used to display configuration map for debugging purpose.
        virtual inline void displayMap(std::map<std::string, std::string> &_configMap)
        {
            if (_configMap.empty())
            {
                _logger->error("Config map is empty.");
                exit(1);
            }
            for (auto &i : _configMap)
            {
                _logger->debug("{} : {}", i.first, i.second);
            }
        }

    protected:
        static std::shared_ptr<spdlog::logger> _logger;
        const std::map<std::string, std::string> _configMap;
    };

} // !namespace mediaDownloader

#endif // !MEDIA_DOWNLOADER_H