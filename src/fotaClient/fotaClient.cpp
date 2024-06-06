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
#include "gpioControl.hh"

flashStatus fotaClient::ecuStatus;
std::mutex fotaClient::ecuStatusMutex;    
std::string fotaClient::fotaStorage;
std::string fotaClient::maxPercent;


bool fotaClient::flashECU(const std::string& ecuType, const std::string& filePath) {
  auto ret = 0;
  int socket_fd;
  setStatus(flashStatus::IDLE);
  auto statusPipe = fotaStorage + "/fifoStatus";

  //config can
  ret = canAdapter::config(socket_fd, ecuFlash.canInterface);
  if (ret == 0) {
    std::cerr << "Failed to config socket CAN" << std::endl;
    return false;
  }

  if (ecuType == "ESP32") {

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
    while(!fotaClient::readESPSignal(socket_fd, signal));
    if (ret == 0) {
      std::cerr << "Failed to read signal" << std::endl;
      setStatus(flashStatus::ERROR);
      return false;
    }

    std::cout << "esp signal: " << signal << std::endl;
    if (signal != (int)STATUSCODE::ERROR_OK) {
      std::cerr << "Failed to connect wifi" << std::endl;
      setStatus(flashStatus::ERROR);
      auto status = ecuType + "_FAILED";
      ret = writeFifoPipe(statusPipe, status);
      if (ret) {
        std::cout << "Successfully to write status" << std::endl;
      } else {
        std::cout << "Failed to write status" << std::endl;
      }
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

    std::atomic<bool> stopFlag(false);
    std::thread receivePercentThread(fotaClient::getFlashStatus, std::ref(socket_fd), ecuType, std::ref(stopFlag));
    ret = restAdapter::sendFileRequest(urlUpdate, filePath);
    stopFlag.store(true);
    receivePercentThread.join();
    if (ret == 0 && maxPercent != "100") {
      setStatus(flashStatus::ERROR);
      auto status = ecuType + "_FAILED";
      ret = writeFifoPipe(statusPipe, status);
      if (ret) {
        std::cout << "Successfully to write status" << std::endl;
      } else {
        std::cout << "Failed to write status" << std::endl;
      }
      std::cerr << "Failed to send firmware's data" << std::endl;
      return false;
    } else {
      setStatus(flashStatus::DONE);
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

    std::cout << "Starting wakeup bootloader" << std::endl;
    fotaClient::wakeupBootloader(ecuType, std::stoi(ecuFlash.reset_pin));
    std::cout << "Finish wakeup bootloader" << std::endl;

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
    } else {
      std::cout << "Successfully to send firmware's size" << std::endl;
    }

    // Send firmware's data
    can_frame data_frame;
    data_frame.can_id = std::stoi(ecuFlash.can_id_Fimware, 0, 16);

    std::atomic<bool> stopFlag(false);
    std::thread receivePercentThread(fotaClient::getFlashStatus, std::ref(socket_fd), ecuType, std::ref(stopFlag));

    for (size_t i = 0; i < dataArray.size(); i += 8) {
      if (!stopFlag.load()) {
        size_t len = std::min<size_t>(8, dataArray.size() - i);
        data_frame.can_dlc = len;
        memcpy(data_frame.data, &dataArray[i], len);
        ret = canAdapter::sendData(socket_fd, data_frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (ret == 0) {
          setStatus(flashStatus::ERROR);
          std::cerr << "Failed to send firmware's data" << std::endl;
          stopFlag.store(true);
          receivePercentThread.join();
          return false;
        } else {
          std::cout << "Successfully to send firmware's data" << std::endl;
        } 
      } else {
        std::cout << "Break send firmware" << std::endl;
        break;
      }
    }
    if (maxPercent != "100") {
      receivePercentThread.join();
      std::cerr << "Failed to send firmware's data" << std::endl;
      auto status = ecuType + "_FAILED";
      ret = writeFifoPipe(statusPipe, status);
      if (ret) {
        std::cout << "Successfully to write status" << std::endl;
      } else {
        std::cout << "Failed to write status" << std::endl;
      }
      return false;
    }
    stopFlag.store(true);
    receivePercentThread.join();
    std::cout << "Successfully to update firmware" << std::endl;
    return true;
  }
}

bool fotaClient::sendESPSignal(int& socket_fd, const can_frame& signalFrame) {
  auto ret = canAdapter::sendData(socket_fd, signalFrame);
  return ret;
}

bool fotaClient::readESPSignal(int& socket_fd, int& signal) {
  can_frame received_frame;
  auto x = canAdapter::readData(socket_fd, received_frame);
  if (x == false) return false;
  signal = (int)received_frame.data[0];
  return true;
}

void fotaClient::config(const ecuInfo& ecuInfor, const std::string& storagePath) {
  ecuFlash = ecuInfor;
  fotaStorage = storagePath;
}

void fotaClient::getFlashStatus(int& socket_fd, const std::string& ecuType, std::atomic<bool>& stopFlag) {
  auto percent = 0;
  auto temp = 100;
  
  while (!stopFlag.load()) {
    if (ecuType == "ESP32") {
      if (fotaClient::readESPSignal(socket_fd, percent) && percent != temp) {
        std::cout << stopFlag.load() << std::endl;
        auto filePath = fotaStorage + "/fifoPercent";
        auto percentStr = ecuType + "_" + std::to_string(percent);
        maxPercent = std::to_string(percent);
        std::cout << "percent pip: " << percentStr << std::endl;
        temp = percent;
        auto ret = fotaClient::writeFifoPipe(filePath, percentStr);
        if (ret) {
          std::cout << "Successfully to write percent" << std::endl;
        } else {
          std::cout << "Failed to write percent" << std::endl;
        }
      } 
    } else {
      auto start_time = std::chrono::steady_clock::now();
      std::chrono::duration<double> timeout;

      while (!fotaClient::readESPSignal(socket_fd, percent) && timeout < std::chrono::seconds(3)) {
        std::cout << "Failed to read percent" << std::endl;
        timeout = std::chrono::steady_clock::now() - start_time;
      }
      
      if (timeout >= std::chrono::seconds(3)) {
        stopFlag.store(true);
      } else {
          if ((percent == 0 || percent == 25 || percent == 50 || percent == 75 || percent == 100) && percent != temp) {
            auto filePath = fotaStorage + "/fifoPercent";
            auto percentStr = ecuType + "_" + std::to_string(percent);
            maxPercent = std::to_string(percent);;
            std::cout << "percent pip: " << percentStr << std::endl;
            temp = percent;
            auto ret = fotaClient::writeFifoPipe(filePath, percentStr);
            if (ret) {
              std::cout << "Successfully to write percent" << std::endl;
            } else {
              std::cout << "Failed to write percent" << std::endl;
            }
          }
      }
    }
  }
  std::cout << "Thread stopped" << std::endl;
}

bool fotaClient::writeFifoPipe(const std::string& fifoPath, std::string& buff) {
  mkfifo(fifoPath.c_str(), 0666);
  int fd = open(fifoPath.c_str(), O_WRONLY | O_NONBLOCK);
  if (fd == -1) {
    std::cerr << "failed to open pipe" << std::endl;
    return false;
  }
  auto ret = write(fd, buff.c_str(), buff.size());
  close(fd);
  if (ret == -1) return false;
  return true;
}

void fotaClient::setStatus(flashStatus status) {
  std::lock_guard<std::mutex> lock(ecuStatusMutex);
  ecuStatus = status;
}

void fotaClient::wakeupBootloader(const std::string& ecu, int pin) {
  GPIO gpio(pin);
  gpio.setModeOutput();
  gpio.setPinLow();

  if (ecu == "ATMEGA328P") {
    usleep(100000);
    gpio.setPinHigh();
    usleep(100000);
    std::cout << "Successfully to wake up bootloader" << std::endl;
  } else {
    usleep(50000);
    gpio.setModeInput();
    usleep(4000000);
  }
}