cmake_minimum_required(VERSION 3.10)

include_directories(../protocolAdapter/canAdapter)
add_subdirectory(../protocolAdapter/canAdapter canAdapterLib)

include_directories(../protocolAdapter/restAdapter)
add_subdirectory(../protocolAdapter/restAdapter restAdapterLib)

include_directories(../gpioControl)
add_subdirectory(../gpioControl gpioLib)

add_library(fotaClient STATIC fotaClient.cpp)
target_link_libraries(fotaClient PUBLIC canAdapter restAdapter gpioControl)
