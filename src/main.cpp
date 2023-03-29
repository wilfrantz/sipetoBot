
#include "sipeto.h"

int main(int argc, char **argv)
{
    spdlog::set_level(spdlog::level::debug);
    sipeto::Sipeto sipeto;
    spdlog::info ("Welcome to SipetoBot {}.", sipeto.getVersion());
    spdlog::info("Social Media Downloader Bot for Telegram.");
    sipeto.getLogger()->debug("Developed by: {}.", sipeto.getAuthor());

    return 0;
}