cmake_minimum_required(VERSION 3.10)
project(testAPP)

include_directories(src/fotaClient)

include_directories(src/application)
add_subdirectory(src/application applicationLib)

include_directories(src/config)
add_subdirectory(src/config configLib)

# Test APP
# add_executable(canTest testapp/testCAN.cpp)
# target_link_libraries(canTest PUBLIC fotaClient)

# add_executable(restTest testapp/testRest.cpp)
# target_link_libraries(restTest PUBLIC fotaClient)

# Test IPC named pipe
# add_executable(fifo1 testapp/testFIFO1.cpp)
# add_executable(fifo2 testapp/testFIFO2.cpp)

# add_executable(testConf testapp/testConfig.cpp)
# target_link_libraries(testConf PUBLIC ecuConfigure)

add_executable(FotaApp src/fota_master.cpp)
target_link_libraries(FotaApp PUBLIC fotaMasterApp fotaClient)