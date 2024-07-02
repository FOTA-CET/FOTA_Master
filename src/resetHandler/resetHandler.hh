#ifndef __RESET_HANDLER_HH
#define __RESET_HANDLER_HH

#include <string>
#include <atomic>

class resetHandler {

  public:
    resetHandler(const std::string& filepath);
    void resetFirmware(std::atomic<bool>& stopFlag);

  private:
    std::string fotaStorage;
    std::string fifoRstECU;
    bool readFifoPipe(const std::string& fifoPath, std::string& buff);

};

#endif