cmake_minimum_required(VERSION 3.10...3.20)
project(sipeto VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_library(TDJSON_LIBRARIES NAMES tdjson HINTS /usr/local)
find_package(Boost REQUIRED COMPONENTS system thread)

OPTION(BUILD_TESTS "Build tests" ON)
OPTION(CMAKE_VERBOSE_MAKEFILE "Verbose Makefile" ON)

add_compile_definitions(ENGINE_VERSION="${PROJECT_VERSION}")

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/sipeto.cpp
)


target_link_libraries(${PROJECT_NAME} PRIVATE
  fmt::fmt
  spdlog::spdlog
  Boost::system
  Boost::thread
  ${TDJSON_LIBRARIES}
)

# if (BUILD_TESTS)
#   message(STATUS "Building tests ...")
#   enable_testing()
#   add_subdirectory(test)
#   add_test(NAME ${PROJECT_NAME}_tests COMMAND test/${PROJECT_NAME}_test)
# endif (BUILD_TESTS)