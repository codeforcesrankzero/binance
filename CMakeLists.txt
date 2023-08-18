cmake_minimum_required(VERSION 3.12)
project(binance VERSION 1.0.0 LANGUAGES CXX)

set(SOURCES main.cpp binance.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")

add_executable(binance ${SOURCES})


set_target_properties(binance PROPERTIES CXX_STANDARD 17)

find_package(CURL REQUIRED)
    include_directories(${CURL_INCLUDE_DIRS})
find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(binance PRIVATE OpenSSL::Crypto ${CURL_LIBRARIES})




