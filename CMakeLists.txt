cmake_minimum_required(VERSION 3.10)
project(testAPP)

include_directories(src/fotaClient)
add_subdirectory(src/fotaClient fotaClientLib)

add_executable(canTest testapp/testCAN.cpp)
target_link_libraries(canTest PUBLIC fotaClient)
add_executable(tastconan testapp/testConan.cpp)

find_package(CURL)
add_executable(restTest testapp/testRest.cpp)
target_link_libraries(restTest PUBLIC CURL::libcurl)

