#ifndef __FOTA_CLIENT_FACTORY__HH
#define __FOTA_CLIENT_FACTORY__HH

#include <queue>
#include <string>
#include <atomic>

struct flashECU {
  std::string name;
  std::string version;
};
class fotaClientFactory {
  public:
    static fotaClientFactory* getInstance(const std::string filepath);
    fotaClientFactory(const fotaClientFactory &obj) = delete;
    fotaClientFactory& operator = (const fotaClientFactory&obj) = delete;
    void listenFlashRequire(std::atomic<bool>& stopFlag);
    bool getNextFlashECU(flashECU& ecu);
    std::queue <flashECU>& getEcuFlashList() {return flashECUList;}
  private:
    fotaClientFactory(const std::string filepath);
    bool readFifoPipe(const std::string& fifoPath, std::string& buff);
    static fotaClientFactory* FTClientFactory;
    std::queue <flashECU> flashECUList;
    std::string fotaStorage;
    std::string fifoECU;
    std::string fifoFlash;
    std::mutex flashECUListMutex;

};
#endif