# Social Media Downloader Bot

![Sipeto](/lander/favicon.ico)

The Social Media Downloader Bot simplifies media downloading from social media links. It offers seamless access to videos, images, and more from platforms like TikTok, Twitter, Instagram, and Facebook. Effortlessly retrieve the media content you need with this powerful tool.

## Table of Contents

- [Social Media Downloader Bot](#social-media-downloader-bot)
  - [Table of Contents](#table-of-contents)
  - [Design](#design)
  - [Prerequisites](#prerequisites)
  - [Progress](#progress)
    - [Structure](#structure)
    - [Build the server](#build-the-server)
    - [Build Integration](#build-integration)
    - [Build the User Interface](#build-the-user-interface)
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
  - [ ] TikTok
  - [ ] Twitter
  - [ ] Instagram
  - [ ] Facebook
- [ ] Build the User Interface.
  - [ ] Telegram
  - [ ] WebApp
- [ ] Testing

### Structure

- Load configuration data into each interface: The configuration data has been successfully integrated into each interface module.
- Implement core functionality: The core functionality of the bot has been implemented, including handling media downloads and interacting with social media APIs.
- Backend & frontend interaction: Establishing the connection and communication between the backend and frontend components of the application

### Build the server

- Building the server is in progress.

### Build Integration

- [ ] Integration with TikTok is pending.
- [ ] Integration with Twitter is pending.
- [ ] Integration with Instagram is pending.
- [ ] Integration with Facebook is pending.

### Build the User Interface

- [ ] User interface development for Telegram is ongoing.
- [ ] User interface development for WebApp is pending.

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

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.