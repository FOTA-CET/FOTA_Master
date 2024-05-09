#include <iostream>
#include <stdint.h>
#include <cstring>
#include <cstddef>
#include <thread>
#include <chrono>

#include "fotaClient.hh"
#include "canAdapter.hh"
#include "restAdapter.hh"

bool fotaClient::flashECU(ECU ecuType, const std::string& filePath) {
  auto ret = 0;
  int socket_fd;
  std::string canInterface = "can0";

  //config can
  ret = canAdapter::config(socket_fd, canInterface);
  if (ret == 0) {
    std::cerr << "Failed to config socket CAN" << std::endl;
    return false;
  }

  if (ecuType == ECU::ESP32) {

    std::string bin_content;
    std::string ip = "192.168.193.9";
    std::string urlUpdate;
    std::string urlFileSize;

    //config url
    restAdapter::configConnection(ip, urlUpdate, urlFileSize);

    ret = restAdapter::readBinFileData(filePath, bin_content);
    if (ret == 0) {
      std::cerr << "Failed to read Bin file data" << std::endl;
      return false;
    }

    //Request connect wifi
    can_frame signalFrame;
    signalFrame.can_id = 0x123;
    signalFrame.can_dlc = 1;
    signalFrame.data[0] = (unsigned char)(CONNECT_CMD::CONNECT);
    ret = sendESPSignal(socket_fd, signalFrame);
    if (ret == 0) {
      std::cerr << "Failed to send request connect wifi" << std::endl;
      return false;
    }

    int signal;
    readESPSignal(socket_fd, signal);
    if (signal != (int)STATUSCODE::ERROR_OK) {
      std::cerr << "Failed to connect wifi" << std::endl;
      return false;
    }

    ret = restAdapter::sendDataRequest(urlFileSize, "fileSize=" + std::to_string(bin_content.size()));
    if (ret == 0) {
      std::cerr << "Failed to send file size" << std::endl;
      return false;
    } else {
      std::cout << "Successfully to send file size" << std::endl;
    }

    ret = restAdapter::sendFileRequest(urlUpdate, filePath);
    if (ret == 0) {
      std::cerr << "Failed to firmware's data" << std::endl;
      return false;
    } else {
      std::cout << "Successfully to send firmware's data" << std::endl;
    }
    return true;

  } else {

    std::vector<uint8_t> dataArray;
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
  std::cout << "Successfully to update firmware" << std::endl;
  return true;
}

bool fotaClient::sendESPSignal(int& socket_fd, const can_frame& signalFrame) {
  auto ret = canAdapter::sendData(socket_fd, signalFrame);
  return ret;
}

bool fotaClient::readESPSignal(int& socket_fd, int& signal) {
  can_frame received_frame;
  while (canAdapter::readData(socket_fd, received_frame) == false) {
      std::cerr << "Failed to read data" << std::endl;
  }
  signal = (int)received_frame.data[0];
  return true;
}
