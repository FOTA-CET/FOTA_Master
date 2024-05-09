#include "linux/can.h"
#include <string>

enum class ECU {
  STM32,
  ATMEGA328P,
  ESP32
};

class fotaClient {

  public:
    bool flashECU(ECU ecuType, const std::string& file);

  private:
    bool sendESPSignal(int& socket_fd, const can_frame& signalFrame);
    bool readESPSignal(int& socket_fd, int& signal);
};