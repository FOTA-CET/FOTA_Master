#ifndef __FOTACLIENT_HH
#define __FOTACLIENT_HH

#include "linux/can.h"
#include <string>

#include "../config/ecu_config.hh"

enum class ECU {
  STM32,
  ATMEGA328P,
  ESP32
};

class fotaClient {

  public:
    bool flashECU(ECU ecuType, const std::string& file);
    void config(const ecuInfo& ecuInfor);
  private:
    bool sendESPSignal(int& socket_fd, const can_frame& signalFrame);
    bool readESPSignal(int& socket_fd, int& signal);
    can_frame canframe;
    ecuInfo ecuFlash;
};
#endif