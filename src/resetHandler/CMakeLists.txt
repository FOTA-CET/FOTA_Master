cmake_minimum_required(VERSION 3.10)

include_directories(../fotaClient)
# add_subdirectory(../fotaClient fotaClientLib)

include_directories(../config)
# add_subdirectory(../config configLib)

add_library(resetHandler STATIC resetHandler.cpp)
# target_link_libraries(resetHandler PUBLIC fotaClientLib)