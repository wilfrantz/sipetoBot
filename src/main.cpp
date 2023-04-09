
#include "sipeto.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

int main(int argc, char **argv)
{
    sipeto::Sipeto sipeto;
    simpleHttpServer::SimpleHttpServer httpServer(sipeto,
                                                  sipeto.getFromConfigMap("address"),
                                                  sipeto.getFromConfigMap("http_port"));

    // Display welcome message
    sipeto.displayGreetings();

    // Start the server
    httpServer.start();

    return 0;
}