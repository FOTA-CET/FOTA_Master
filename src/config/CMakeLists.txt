cmake_minimum_required(VERSION 3.10)

add_library(ecuConfigure STATIC ecu_config.cpp)

find_package(RapidJSON REQUIRED)
target_link_libraries(ecuConfigure PUBLIC rapidjson)