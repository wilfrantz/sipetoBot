
#include "simple_http_server.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

int main(int argc, char **argv)
{
    simpleHttpServer::SimpleHTTPServer server;
    server.setWebhook();

    sipeto::Sipeto sipeto;
    std::thread{[&sipeto]
                { sipeto.startServer(sipeto.getFromConfigMap("address"),
                                     sipeto.getFromConfigMap("port")); }}
        .detach();

    // Wait for an update to be received
    std::unique_lock<std::mutex> lock(sipeto.receivedUpdateMutex);
    while (sipeto.receivedUpdate.empty())
    {
        sipeto.updateReceived.wait(lock);
    }

    // Print the received update
    spdlog::info("Received update: {}", sipeto.receivedUpdate);

    return 0;
}