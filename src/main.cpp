
#include "sipeto.h"

using namespace sipeto;
using namespace simpleHttpServer;

int main(int argc, char **argv)
{
    Sipeto sipeto;
    SimpleHttpServer httpServer(sipeto);

    // Display welcome message
    sipeto.displayGreetings();

    // Start the server
    httpServer.start();

    return 0;
}