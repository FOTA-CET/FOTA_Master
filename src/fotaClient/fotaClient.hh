#ifndef __FOTACLIENT_HH
#define __FOTACLIENT_HH

#include "linux/can.h"
#include <string>
#include <mutex>
#include <atomic>

#include "../config/ecu_config.hh"

enum class ECU {
  STM32,
  ATMEGA328P,
  ESP32
};

enum class flashStatus {
  IDLE,
  FLASHING,
  ERROR,
  DONE
};

class fotaClient {

  public:
    bool flashECU(ECU ecuType, const std::string& file);
    void config(const ecuInfo& ecuInfor, const std::string& storagePath);
    static void getFlashStatus(int& socket_fd);
    static void setStatus(flashStatus status);
    // static flashStatus getStatus();
  private:
    bool sendESPSignal(int& socket_fd, const can_frame& signalFrame);
    static bool readESPSignal(int& socket_fd, int& signal);
    static bool writeFifoPipe(const std::string& fifoPath, std::string& buff);
    can_frame canframe;
    ecuInfo ecuFlash;
    static std::string fotaStorage;
    static flashStatus ecuStatus;
    static std::mutex ecuStatusMutex;
};
#endif