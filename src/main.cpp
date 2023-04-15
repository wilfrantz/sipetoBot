#include "sipeto.h"

using namespace std;
using namespace sipeto;
using namespace boost::asio;
using namespace simpleHttpServer;

int main(int argc, char **argv)
{
    Sipeto sipeto;
    // set log level (debug, info, warn, error, critical, or off)
    sipeto.setLogLevel(sipeto.getFromConfigMap("mode"));

    // Display program information message
    sipeto.displayInfo();

    boost::asio::io_context ioc{1};
    auto address = ip::make_address(sipeto.getFromConfigMap("address"));
    auto port = static_cast<unsigned short>(std::atoi(sipeto.getFromConfigMap("port").c_str()));
    tcp::endpoint endpoint{address, port};

    SimpleHttpServer server(ioc, endpoint, sipeto);
    // Start the http server
    server.start();

    return 0;
}
