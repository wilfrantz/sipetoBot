
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <string>
#include <thread>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "sipeto.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

// Define a global variable to hold the received update
// std::atomic<std::string> receivedUpdate("");

// Define a global variable to signal when an update has been received
// std::condition_variable updateReceived;
// std::mutex updateMutex;
// bool updateAvailable = false;

int main(int argc, char **argv)
{
    spdlog::set_level(spdlog::level::debug);

    sipeto::Sipeto sipeto;
    // Start the server on port 8080
    std::thread{startServer, "0.0.0.0", "8080"}.detach();

    // Wait for an update to be received
    std::unique_lock<std::mutex> lock(sipeto.updateMutex);
    while (!sipeto.updateAvailable)
    {
        sipeto.updateReceived.wait(lock);
    }

    sipeto.readInput();

    return 0;
}