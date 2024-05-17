#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <mutex>

#include "fotaClientFactory.hh"
fotaClientFactory* fotaClientFactory::FTClientFactory;

fotaClientFactory* fotaClientFactory::getInstance(const std::string filepath) {
  if (FTClientFactory == nullptr) {
    FTClientFactory = new fotaClientFactory(filepath);
    
  }
  return FTClientFactory;
}

fotaClientFactory::fotaClientFactory(const std::string filepath) {
  fotaStorage = filepath;
  fifoECU = fotaStorage + "/fifoECU";
  fifoFlash = fotaStorage + "/fifoFlash";
  std::cout << "fifoECU: " << fifoECU << std::endl;
  std::cout << "fifoFlash: " << fifoFlash << std::endl;
}

void fotaClientFactory::listenFlashRequire(std::atomic<bool>& stopFlag) {
  std::string nameECUFlash;
  std::string firmware;
  while (!stopFlag.load()) {
    std::cout << "listenFlashRequire..." << std::endl;
    if (fotaClientFactory::readFifoPipe(fifoECU, nameECUFlash) && 
        fotaClientFactory::readFifoPipe(fifoFlash, firmware)) {
      flashECU ecu{nameECUFlash, firmware};
      std::cout << "nameECUFlash: " << nameECUFlash << std::endl;
      std::cout << "firmware: " << firmware << std::endl;
      std::lock_guard<std::mutex> lock(flashECUListMutex);
      flashECUList.push(ecu);
      std::cout << "Succesfully to add to flashECUList, size: " << flashECUList.size() << std::endl;
    }
  }
}

bool fotaClientFactory::readFifoPipe(const std::string& fifoPath, std::string& buff) {
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

bool fotaClientFactory::getNextFlashECU(flashECU& ecu) {
  std::lock_guard<std::mutex> lock(flashECUListMutex);
  if (flashECUList.empty()) {
    return false;
  }
  ecu = flashECUList.front();
  std::cout << "get getNextFlashECU: " << ecu.name << std::endl;
  flashECUList.pop();
  std::cout << "size: " << flashECUList.size() << std::endl;
  return true;
}

