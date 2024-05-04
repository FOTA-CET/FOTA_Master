#include "iostream"
#include "fotaClient.hh"

int main() {
  std::cout << "------------------------------------------" << std::endl;
  std::cout << "|  Test app update firmware via Rest APIs|" << std::endl;
  std::cout << "------------------------------------------" << std::endl;

  fotaClient client;
  std::string filePath = "/home/nuu9hc/Documents/app.ino.bin";
  auto ret = client.flashECU(ECU::ESP32, filePath);
  if (ret == 1) {
    std::cout << "Successfully to flash ESP32" << std::endl;
  } else {
    std::cout << "Failed to flash ESP32" << std::endl;
  }
}