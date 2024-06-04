#include <cstdlib>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <csignal>

#include "fota_master_app.hh"
#include "ecu_config.hh"
#include "fotaClientFactory.hh"

std::atomic<bool> fotaMasterApp::stopFlag{false};
std::thread fotaMasterApp::listFlashRequireThread;

fotaMasterApp::fotaMasterApp() {

  if (std::getenv("FOTA_STORAGE") == nullptr) {
    auto errMsg = "fotaMasterApp: environment variable FOTA_STORAGE is not set";
    throw std::runtime_error(errMsg);
  } else {
    fotaStorage = std::getenv("FOTA_STORAGE");
    fota_conf = fotaStorage + "/config";
    ecuConfigFile = fota_conf + "/ecu.config";
  }
  std::cout << "fotaStorage: " << fotaStorage << std::endl;
}

void fotaMasterApp::configure() {
  EcuConfig::Parse(ecuConfigFile);
}

void fotaMasterApp::start() {
  std::string nameECUFlash;
  std::string firmware;
  flashECU requireFlashEcu;
  
  fotaClientFactory* FTClientFactory = fotaClientFactory::getInstance(fotaStorage);
  listFlashRequireThread = std::thread(&fotaClientFactory::listenFlashRequire, FTClientFactory, std::ref(stopFlag));

  while (true)
  {
    if (FTClientFactory->getNextFlashECU(requireFlashEcu)) {
    
      nameECUFlash = requireFlashEcu.name;
      firmware = requireFlashEcu.version;
      std::cout << "Starting Flash " << nameECUFlash << std::endl;

      auto ecuFlashInfo = EcuConfig::getEcuInfo(nameECUFlash);

      fotaClient mfotaClient;
      // auto ECU = fotaMasterApp::convertEcuString(nameECUFlash);
      auto filePath = fotaStorage + "/firmware/" + firmware;
      std::cout << "filePath: " << filePath << std::endl;
      mfotaClient.config(ecuFlashInfo, fotaStorage);
      auto ret = mfotaClient.flashECU(nameECUFlash, filePath);

      signal(SIGINT, fotaMasterApp::signalHandler);
      signal(SIGTERM, fotaMasterApp::signalHandler);
    }
  }
}

void fotaMasterApp::signalHandler(int signal) {
  std::cout << "Received signal " << signal << std::endl;
  stopFlag.store(true);
  listFlashRequireThread.join();
  exit(signal);
}

