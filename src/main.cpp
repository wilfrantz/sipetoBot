#include "include/sipeto.h"
#include "include/tiktok.h"
#include "include/twitter.h"
#include "include/instagram.h"

using namespace std;
using namespace sipeto;
using namespace tiktok;
using namespace twitter;
using namespace instagram;
using namespace boost::asio;
using namespace simpleHttpServer;

int main(int argc, char **argv)
{
    Sipeto sipeto;
    // load configuration map.
    sipeto.loadConfig();

    /// NOTE: set log level (debug, info, warn, error, critical, or off)
    sipeto.setLogLevel(sipeto.getFromConfigMap("mode"));
    sipeto.displayInfo();

    TikTok tiktok;
    tiktok.displayMap(tiktok.getTheMap());
    exit(0);

    boost::asio::io_context ioc{1};
    auto address = ip::make_address(sipeto.getFromConfigMap("address"));
    auto port = static_cast<unsigned short>(std::atoi(sipeto.getFromConfigMap("port").c_str()));
    tcp::endpoint endpoint{address, port};

    SimpleHttpServer server(ioc, endpoint, sipeto);

    // Start the http server
    server.start();

    return 0;
}
