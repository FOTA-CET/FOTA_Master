#include <string>

#include "fotaClient.hh"

class fotaMasterApp {
  public:
    fotaMasterApp();
    void configure();
    void start();
    ECU convertEcuString(const std::string& ecuName);

  private:
    static void signalHandler(int signal);
    bool readFifoPipe(const std::string& fifoPath, std::string& buff);
    std::string fota_conf;
    std::string ecuConfigFile;
    std::string fotaStorage;
    std::string fifoECU;
    std::string fifoFlash;
    
};