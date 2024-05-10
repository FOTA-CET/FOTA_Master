#ifndef __ECU_CONFIG_HH
#define __ECU_CONFIG_HH

#include <string>
#include <vector>


struct ecuInfo
{
  std::string name;
  std::string canInterface;
  std::string can_id_size;
  std::string can_id_Fimware;
  std::string ip;
};

class EcuConfig {
  public:
    static void Parse(const std::string filePath);
    static ecuInfo getEcuInfo(std::string name);

  private:
    static std::vector<ecuInfo> ecuInfoList;
};
#endif
