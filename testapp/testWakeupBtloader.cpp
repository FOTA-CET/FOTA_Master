#include <iostream>

#include "fotaClient.hh"

int main() {
    std::cout << "Test wakeup bootloader" << std::endl;
    std::string ecu = "ATMEGA328P";
    int pin = 23;
    fotaClient::wakeupBootloader(ecu, pin);
    return 0;
}