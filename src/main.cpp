
#include "sipeto.h"

using namespace sipeto;
using namespace simpleHttpServer;

int main(int argc, char **argv)
{
    Sipeto sipeto;
    // debug, info, warn, error, critical, off
    sipeto.setLogLevel(sipeto.getFromConfigMap("mode"));
    // Display program information message
    sipeto.displayInfo();

    SimpleHttpServer httpServer(sipeto);
    // Start the http server
    httpServer.start();

    return 0;
}