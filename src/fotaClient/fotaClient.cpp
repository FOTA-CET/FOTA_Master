#include <iostream>
#include <stdint.h>
#include <cstring>
#include <cstddef>
#include <thread>
#include <chrono>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <atomic>


#include "fotaClient.hh"
#include "canAdapter.hh"
#include "restAdapter.hh"

flashStatus fotaClient::ecuStatus;
std::mutex fotaClient::ecuStatusMutex;    
std::string fotaClient::fotaStorage;


bool fotaClient::flashECU(ECU ecuType, const std::string& filePath) {
  auto ret = 0;
  int socket_fd;
  setStatus(flashStatus::IDLE);

  //config can
  ret = canAdapter::config(socket_fd, ecuFlash.canInterface);
  if (ret == 0) {
    std::cerr << "Failed to config socket CAN" << std::endl;
    return false;
  }

  if (ecuType == ECU::ESP32) {

    std::string bin_content;
    std::string urlUpdate;
    std::string urlFileSize;

    //config url
    restAdapter::configConnection(ecuFlash.ip, urlUpdate, urlFileSize);

    ret = restAdapter::readBinFileData(filePath, bin_content);
    if (ret == 0) {
      std::cerr << "Failed to read Bin file data" << std::endl;
      return false;
    }

    setStatus(flashStatus::FLASHING);
    //Request connect wifi
    can_frame signalFrame;
    signalFrame.can_id = std::stoi(ecuFlash.can_id_Fimware, 0, 16);
    signalFrame.can_dlc = 1;
    signalFrame.data[0] = (unsigned char)(CONNECT_CMD::CONNECT);
    ret = sendESPSignal(socket_fd, signalFrame);
    if (ret == 0) {
      std::cerr << "Failed to send request connect wifi" << std::endl;
      setStatus(flashStatus::ERROR);
      return false;
    }

    int signal;
    fotaClient::readESPSignal(socket_fd, signal);
    if (signal != (int)STATUSCODE::ERROR_OK) {
      std::cerr << "Failed to connect wifi" << std::endl;
      setStatus(flashStatus::ERROR);
      return false;
    }

    ret = restAdapter::sendDataRequest(urlFileSize, "fileSize=" + std::to_string(bin_content.size()));
    if (ret == 0) {
      std::cerr << "Failed to send file size" << std::endl;
      setStatus(flashStatus::ERROR);
      return false;
    } else {
      std::cout << "Successfully to send file size" << std::endl;
    }

    // std::atomic<bool> stopFlag(false);
    std::thread receivePercentThread(fotaClient::getFlashStatus, std::ref(socket_fd));
    ret = restAdapter::sendFileRequest(urlUpdate, filePath);
    std::cout << "ret= " << ret << std::endl;
    if (ret == 0) {
      setStatus(flashStatus::ERROR);
      receivePercentThread.join();
      std::cerr << "Failed to send firmware's data" << std::endl;
      return false;
    } else {
      setStatus(flashStatus::DONE);
      receivePercentThread.join();
      std::cout << "Successfully to send firmware's data" << std::endl;
    }
    return true;

  } else {

    std::vector<uint8_t> dataArray;
    ret = canAdapter::readHexFileData(filePath, dataArray);
    if (ret == 0) {
      setStatus(flashStatus::ERROR);
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
    size_frame.can_id = std::stoi(ecuFlash.can_id_size, 0, 16);;
    size_frame.can_dlc = digitArray.size();
    memcpy(size_frame.data, digitArray.data(), digitArray.size());
    ret = canAdapter::sendData(socket_fd, size_frame);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    if (ret == 0) {
      std::cerr << "Failed to send firmware's size" << std::endl;
    }

    // Send firmware's data
    can_frame data_frame;
    data_frame.can_id = std::stoi(ecuFlash.can_id_Fimware, 0, 16);;
    for (size_t i = 0; i < dataArray.size(); i += 8) {
      size_t len = std::min<size_t>(8, dataArray.size() - i);
      data_frame.can_dlc = len;
      memcpy(data_frame.data, &dataArray[i], len);
      ret = canAdapter::sendData(socket_fd, data_frame);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (ret == 0) {
        setStatus(flashStatus::ERROR);
        std::cerr << "Failed to send firmware's data" << std::endl;
        return false;
      }

      // Read percent of flashing
      can_frame received_frame;
      while(!canAdapter::readData(socket_fd, received_frame)) {
        std::cerr << "Failed to read percent of flashing" << std::endl;
      }
      auto percent = (int)received_frame.data[0];
      auto percentStr = std::to_string(percent);
      std::cout << "Percent Flash: " << percent << std::endl;
      std::string filePath = fotaStorage + "/percent";
      std::cout << "percent pip: " << filePath << std::endl;
      fotaClient::writeFifoPipe(filePath, percentStr);
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

void fotaClient::config(const ecuInfo& ecuInfor, const std::string& storagePath) {
  ecuFlash = ecuInfor;
  fotaStorage = storagePath;
}

void fotaClient::getFlashStatus(int& socket_fd) {
  auto percent = 0;
  auto temp = 100;
  while (percent != 100) {
    fotaClient::readESPSignal(socket_fd, percent);
    if (percent != temp) {
      auto filePath = fotaStorage + "/percent";
      auto percentStr = std::to_string(percent);
      std::cout << "percent pip: " << percentStr << std::endl;
      auto ret = fotaClient::writeFifoPipe(filePath, percentStr);
      temp = percent;
    } 
  }
}

bool fotaClient::writeFifoPipe(const std::string& fifoPath, std::string& buff) {
  mkfifo(fifoPath.c_str(), 0666);
  int fd = open(fifoPath.c_str(), O_WRONLY);
  auto ret = write(fd, buff.c_str(), sizeof(buff));
  close(fd);
  if (ret == -1) return false;
  return true;
}

void fotaClient::setStatus(flashStatus status) {
  std::lock_guard<std::mutex> lock(ecuStatusMutex);
  ecuStatus = status;
}

// flashStatus fotaClient::getStatus() {
//   return ecuStatus;
// }