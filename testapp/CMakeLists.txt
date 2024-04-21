cmake_minimum_required(VERSION 3.10)
project(canTest)

# set AVR Compiler
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-none-linux-gnueabihf-g++)

include_directories(../src/fotaClient)
add_subdirectory(../src/fotaClient fotaClientLib)

add_executable(canTest testCAN.cpp)
target_link_libraries(canTest PUBLIC fotaClient)
