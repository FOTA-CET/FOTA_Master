#include <iostream>
#include <stdint.h>
#include <cstring>
#include <cstddef>
#include <thread>
#include <chrono>

#include "fotaClient.hh"
#include "canAdapter.hh"

bool fotaClient::flashECU(ECU ecuType, const std::string& filePath) {
  auto ret = 0;
  int socket_fd;
  std::string canInterface = "can0";
  std::vector<uint8_t> dataArray;

  if (ecuType == ECU::ESP32){

  } else {
    ret = canAdapter::config(socket_fd, canInterface);
    if (ret == 0) {
      std::cerr << "Failed to config socket CAN" << std::endl;
      return false;
    }

    ret = canAdapter::readHexFileData(filePath, dataArray);
    if (ret == 0) {
      std::cerr << "Failed to read hex file data" << std::endl;
      return false;
    }

    // Send firmware's size
    can_frame size_frame;
    std::vector<uint8_t> digitArray;
    std::string firmwareSizeStr = std::to_string(dataArray.size());
      for (auto digit : firmwareSizeStr) {
      digitArray.push_back(digit - '0');
    }

    // Arbitration ID
    size_frame.can_id = 0x01;
    size_frame.can_dlc = digitArray.size();
    memcpy(size_frame.data, digitArray.data(), digitArray.size());
    ret = canAdapter::sendData(socket_fd, size_frame);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    if (ret == 0) {
      std::cerr << "Failed to send firmware's size" << std::endl;
    }

    // Send firmware's data
    can_frame data_frame;
    data_frame.can_id = 0x02;
    for (size_t i = 0; i < dataArray.size(); i += 8) {
      size_t len = std::min<size_t>(8, dataArray.size() - i);
      data_frame.can_dlc = len;
      memcpy(data_frame.data, &dataArray[i], len);
      ret = canAdapter::sendData(socket_fd, data_frame);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (ret == 0) {
        std::cerr << "Failed to send firmware's data" << std::endl;
      }
    }
  }
  return true;
}