# Social Media Downloader Bot using C++ and Telegram API

### Introduction
The Social Media Downloader Bot is a C++ program that utilizes the Telegram Bot API to interact with users and allow them to download images and videos from Twitter, Facebook, and Instagram. This document outlines the design and structure of the program, including its main components, dependencies, and interactions.


### Dependencies

*  Twitter API
*  Boost library
*  Facebook Graph API
*  Instagram Private API
*  C++ (minimum version 17)
*  tdlib - Telegram Bot API library.
*  JSONCPP for Modern C++ library (JSON parsing)


### Program Components
The program consists of the following main components:

#### Telegram Bot API Interface
*  Handles user input and output, i.e., sending and receiving messages from users.
*  Provides methods to send images and videos to users.

#### Social Media API Handlers
*  Interfaces with the respective social media APIs (Facebook, Twitter, and Instagram) to fetch the required media (images and videos).
*  Handles authentication and API rate limits.

#### Media Downloader
*  Downloads the media (images and videos) using libcurl and stores them temporarily.

#### URL Parser
*  Extracts the relevant URLs for media content from the provided social media post URLs.

#### Main Program
*  Combines all components and manages their interactions.

#### Program Flow
The user sends a command to the bot with a link to a Twitter, Facebook, or Instagram post. The bot parses the URL and passes it to the relevant social media API handler. The API handler authenticates with the API and fetches the media URL(s) for the post. The bot then passes the media URL(s) to the media downloader, which downloads the media and temporarily stores it. The bot sends the downloaded media to the user via the Telegram Bot API interface.
