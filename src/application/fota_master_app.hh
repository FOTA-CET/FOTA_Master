#include <string>
#include <thread>

#include "fotaClient.hh"

class fotaMasterApp {
  public:
    fotaMasterApp();
    void configure();
    void start();

  private:
    static void signalHandler(int signal);
    std::string fota_conf;
    std::string ecuConfigFile;
    std::string fotaStorage;
    static std::atomic<bool> stopFlag;
    static std::thread listFlashRequireThread;

};