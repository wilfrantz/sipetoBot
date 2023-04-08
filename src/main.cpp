
#include "simple_http_server.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

int main(int argc, char **argv)
{
    sipeto::Sipeto sipeto;
    // Display welcome message
    sipeto.displayGreetings();

    // start the server
    std::thread serverThread{[&sipeto]
                             { sipeto.startServer(); }};
    // .detach();

    // wait for the server to start
    while (!sipeto.isServerRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
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

    // Join the server thread
    serverThread.join();
    return 0;
}