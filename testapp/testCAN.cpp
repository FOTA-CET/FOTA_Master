#include "iostream"
#include "../src/fotaClient/fotaClient.hh"

int main() {

  std::cout << "-------------------------------------" << std::endl;
  std::cout << "|  Test app update firmware via CAN  |" << std::endl;
  std::cout << "-------------------------------------" << std::endl;

  fotaClient client;
  std::string filePath = "/home/nuu9hc/Documents/sourceC/program.hex";
  auto ret= client.flashECU(ECU::ATMEGA328P, filePath);
  if (ret == 1) {
    std::cout << "Successfully to flash ATMEGA328P" << std::endl;
  } else {
    std::cout << "Failed to flash ATMEGA328P" << std::endl;
  }
}
