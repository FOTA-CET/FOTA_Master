cmake_minimum_required(VERSION 3.10)

include_directories(../fotaClient)
add_subdirectory(../fotaClient fotaClientLib)

include_directories(../config)

include_directories(../fotaClientFactory)
add_subdirectory(../fotaClientFactory clientFactoryLib)

add_library(fotaMasterApp STATIC fota_master_app.cpp)
target_link_libraries(fotaMasterApp PUBLIC fotaClient ecuConfigure fotaClientFactory)