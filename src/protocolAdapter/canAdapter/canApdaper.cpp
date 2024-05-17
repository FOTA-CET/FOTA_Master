
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <cstdlib>
#include <iostream>

#include "canAdapter.hh"

bool canAdapter::readHexFileData(const std::string& file_path, std::vector<uint8_t>& dataArray) {
  std::string line;
  std::string recodeType;
  std::string data;
  
  std::ifstream file(file_path);
  if (!file.is_open()) {
    return false;
  }

  while (getline(file, line)) {
    // Skip ':' in hex file
    line = line.substr(1);
    recodeType = line.substr(6,2);
    if (recodeType == "00") {
      data = line.substr(8, line.length() - 11);
      for (auto i = 0; i < data.length(); i+=2) {
        auto x = data.substr(i,2);
        auto intData = (uint8_t)(std::stoi(x,nullptr,16));
        dataArray.push_back(intData);
      }
    }
  }
  return true;
}

bool canAdapter::config(int& socket_fd, const std::string& canInterface) {

  struct ifreq ifr;
  struct sockaddr_can addr;

  system("sudo ip link set can0 up type can bitrate 500000 triple-sampling on");

  // Open a CAN socket
  socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (socket_fd < 0) {
    return false;
  }

  // Get interface index
  strcpy(ifr.ifr_name, canInterface.c_str());
  if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
    close(socket_fd);
    return false;
  }

  // Bind socket to the CAN interface
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(socket_fd);
    return false;
  }
  return true;
}

bool canAdapter::checkConnect() {
  return true;
}

bool canAdapter::sendData(int& socket_fd, const can_frame& data_frame) {
  if (write(socket_fd, &data_frame, sizeof(data_frame)) == -1) {
    return false;
  }
  return true;
}

bool canAdapter::readData(int socket_fd, can_frame& received_frame) {
  fd_set read_fds;
  struct timeval timeout;

  FD_ZERO(&read_fds);
  FD_SET(socket_fd, &read_fds);

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  int rv = select(socket_fd + 1, &read_fds, nullptr, nullptr, &timeout);
  if (rv <= 0) return false;

  auto x = read(socket_fd, &received_frame, sizeof(received_frame));

  if(x < 0) {
    return false;
  }
  return true;
}
