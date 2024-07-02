#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#include "resetHandler.hh"
#include "ecu_config.hh"
#include "fotaClient.hh"

resetHandler::resetHandler(const std::string& filepath) {
  fotaStorage = filepath;
  fifoRstECU = fotaStorage + "/fifoReset";
  std::cout << "fifoReset: " << fifoRstECU << std::endl;
}

void resetHandler::resetFirmware(std::atomic<bool>& stopFlag) {
  std::string ecuReset;
  while (!stopFlag.load()) {
    if (resetHandler::readFifoPipe(fifoRstECU, ecuReset)) {
      auto ecuResetInfo = EcuConfig::getEcuInfo(ecuReset);
      fotaClient mfotaClient;
      mfotaClient.config(ecuResetInfo, fotaStorage);
      auto ret = mfotaClient.resetFirmware(ecuReset);
    }
  }
}

bool resetHandler::readFifoPipe(const std::string& fifoPath, std::string& buff) {
  char buffer[100];
  auto fd = open(fifoPath.c_str(), O_RDONLY);
  if (fd == -1) {
    return false;
  }

  auto ret = read(fd, &buffer, sizeof(buffer));
  if (ret <= 0) {
    return false;
  }
  buff = buffer;
  close(fd);
  return true;
}


