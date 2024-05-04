cmake_minimum_required(VERSION 3.10)
add_library(restAdapter STATIC restAdapter.cpp)

find_package(CURL)
target_link_libraries(restAdapter PUBLIC CURL::libcurl)