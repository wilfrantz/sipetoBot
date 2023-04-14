
#include "sipeto.h"

using namespace sipeto;
using namespace simpleHttpServer;

int main(int argc, char **argv)
{
    Sipeto sipeto;
    // set log level (debug, info, warn, error, critical, or off)
    sipeto.setLogLevel(sipeto.getFromConfigMap("mode"));
    // Display program information message
    sipeto.displayInfo();

    SimpleHttpServer httpServer(sipeto);
    // Start the http server
    httpServer.start();

    return 0;
}