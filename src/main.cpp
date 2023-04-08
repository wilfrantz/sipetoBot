
#include "simple_http_server.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

int main(int argc, char **argv)
{
    sipeto::Sipeto sipeto;
    // Display welcome message
    sipeto.displayGreetings();

    // start the server
    std::thread{[&sipeto]
                { sipeto.startServer(); }}
        .detach();

    // Wait for an update to be received
    std::unique_lock<std::mutex> lock(sipeto.receivedUpdateMutex);
    while (sipeto.receivedUpdate.empty())
    {
        sipeto.updateReceived.wait(lock);
        spdlog::info("Waiting for update...");
    }

    // Process the received update
    std::string responseBody = sipeto.processRequest(sipeto.receivedUpdate);
    spdlog::info("Received update: {}", responseBody);

    return 0;
}