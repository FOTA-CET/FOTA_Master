#ifndef __FOTACLIENT_HH
#define __FOTACLIENT_HH

#include "linux/can.h"
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

#include "../config/ecu_config.hh"


enum class flashStatus {
  IDLE,
  FLASHING,
  ERROR,
  DONE
};

class fotaClient {

  public:
    bool flashECU(const std::string& ecuType, const std::string& file);
    void config(const ecuInfo& ecuInfor, const std::string& storagePath);
    static void getFlashStatus(int& socket_fd, const std::string& ecuType, std::atomic<bool>& stopFlag);
    static void setStatus(flashStatus status);
    static void wakeupBootloader(const std::string& ecu, int pin);
    // static flashStatus getStatus();
  private:
    bool sendESPSignal(int& socket_fd, const can_frame& signalFrame);
    static bool readESPSignal(int& socket_fd, int& signal);
    static bool writeFifoPipe(const std::string& fifoPath, std::string& buff);
    can_frame canframe;
    ecuInfo ecuFlash;
    static std::string maxPercent;
    static std::string fotaStorage;
    static flashStatus ecuStatus;
    static std::mutex ecuStatusMutex;
    
};
#endif