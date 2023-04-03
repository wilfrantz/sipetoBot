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
#include <thread>

#include "sipeto.h"
#include "simple_http_server.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

int main(int argc, char **argv)
{
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