#ifndef HEADER_HPP
#define HEADER_HPP

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#define F_OK 0
#define access _access
#endif // !_WIN32

#ifdef __APPLE__
#include <json/json.h>
#include <json/reader.h>

#else
#include <iostream>
#include <filesystem>
#include <curl/curl.h>
#include <bits/stdc++.h>

#include <spdlog/spdlog.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>


#endif // !__APPLE__

#include "td_header.h"

#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <curl/curl.h>
#include <boost/asio.hpp>
#include <cstdlib>
#include <thread>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <spdlog/spdlog.h>
#include <boost/regex.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/algorithm/string.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>




#endif // !HEADER_HPP