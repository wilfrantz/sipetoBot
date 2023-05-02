# Social Media Downloader Bot

The Social Media Downloader Bot is a powerful tool for downloading media from social media links.

## Prerequisites

To build this program, you must have the following dependencies installed:

* CMake (version 3.10... or later)
* C++ compiler with C++17 support
* Boost (version 1.67 or later)
* fmt (version 7.1.3 or later)
* spdlog (version 1.5.0 or later)
* libcurl (version 7.68.0 or later)
* JSONCPP (version 1.9.5 or later)
* TDLib (version 1.7.0 or later)

**To install these dependencies, run the following commands in your terminal:**

``` bash
 sudo apt update 
 ```

``` bash
 sudo apt install libfmt-dev libspdlog-dev libjsoncpp-dev libboost-system-dev libboost-thread-dev libcurl4-openssl-dev libboost-all-dev  -y
 ```

**or use homebrew:**

``` bash
 brew update 
 ```

``` bash
 brew install fmt spdlog jsoncpp boost curl-openssl 
 ```

Once the dependencies are installed, you can compile and run the Social Media Downloader Bot to start downloading media from social media links.

## Building

To build sipeto, follow these steps:

1- Clone this repository to your local machine.

2- Open a terminal and navigate to the directory where you cloned the repository.

3- Create a build directory inside the repository directory and navigate to it:

``` bash
mkdir build
cd build
```

4- Run `cmake` to genrate build files:

``` bash
cmake ..
```

5- Build the project.

``` bash
cmake --build ..
```

## Usage

Once you have built the sipeto program, you can use it to download media from social media platforms. To do this, run the program and provide a valid URL to a social media post. You can also specify the media type you want to download, such as video or image.

## Contribution

If you would like to contribute to this project, please submit a pull request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
