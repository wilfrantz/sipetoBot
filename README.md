# Sipeto© simplifies media downloading from social media

![Sipeto](/lander/favicon.ico)

The Social Media Downloader Bot simplifies media downloading from social media links. It offers seamless access to videos, images, and more from platforms like TikTok, Twitter, Instagram, and Facebook. Effortlessly retrieve the media content you need with this powerful tool.

## Table of Contents

- [Sipeto© simplifies media downloading from social media](#sipeto-simplifies-media-downloading-from-social-media)
  - [Table of Contents](#table-of-contents)
  - [Design](#design)
  - [Prerequisites](#prerequisites)
  - [Progress](#progress)
  - [Build the program](#build-the-program)
  - [Usage](#usage)
  - [Contribution](#contribution)
  - [License](#license)

## Design

Read the [design document](https://dede.dev/posts/Media-downloader-telegramBot-design-document/) for a detailed overview of the bot's design.

## Prerequisites

Before building the program, ensure that you have the following dependencies installed:

- CMake (version 3.10 or later)
- C++ compiler with C++17 support
- Boost (version 1.67 or later)
- fmt (version 7.1.3 or later)
- spdlog (version 1.5.0 or later)
- libcurl (version 7.68.0 or later)
- JSONCPP (version 1.9.5 or later)
- TDLib (version 1.7.0 or later)

To install these dependencies, run the following commands in your terminal:

```bash
sudo apt update 
sudo apt install libfmt-dev libspdlog-dev libjsoncpp-dev libboost-system-dev libboost-thread-dev libcurl4-openssl-dev libboost-all-dev -y
```

Alternatively, you can use Homebrew (macOS package manager) to install the dependencies:

```bash
brew update 
brew install fmt spdlog jsoncpp boost curl-openssl
```

Once the dependencies are installed, you can proceed to build and run the Social Media Downloader Bot to start downloading media from social media links.

## Progress

- [x] Structure
  - [x] Load configuration data into each interface.
  - [x] Implement core functionality.
  - [ ] Backend & frontend interaction.
- [ ] Build the server.
- [ ] Build Integration
  - [ ] TikTok (in progress)
  - [ ] Twitter
  - [ ] Instagram
  - [ ] Facebook
- [ ] Build the User Interface.
  - [ ] Telegram
  - [ ] WebApp
- [ ] Testing

## Build the program

To build the "sipeto" program, follow these steps:

1. Clone this repository to your local machine.
2. Open a terminal and navigate to the directory where you cloned the repository.
3. Create a build directory inside the repository directory and navigate to it:

```bash
mkdir build
cd build
```

4. Run `cmake` to generate the build files:

```bash
cmake ..
```

5. Build the project:

```bash
cmake --build .
```

## Usage

Once you have built the "sipeto" program, you can use it to download media from social media platforms. Run the program and provide a valid URL to a social media post. You can also specify the media type you want to download, such as video or image.

> Note: This project is still under construction. More features and improvements are being added.

For more information, please refer to the [documentation](https://dede.dev/posts/Media-downloader-telegramBot-design-document/).

## Contribution

If you would like to contribute to this project, please submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE.txt) file for details.
