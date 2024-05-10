#include <iostream>

#include <ecu_config.hh>

int main() {
  std::string filePath = "/home/nuu9hc/WorkSpace/FOTA_Master/FOTA_Master/conf/ecu.config";
  EcuConfig::Parse(filePath);
  auto ecuInfor = EcuConfig::getEcuInfo("ATMEGA328P");
  std::cout << ecuInfor.name << std::endl;
  std::cout << ecuInfor.canInterface << std::endl;
  std::cout << ecuInfor.can_id_size << std::endl;
  std::cout << ecuInfor.can_id_Fimware << std::endl;
  // std::cout << ecuInfor.ip << std::endl;
  return 0;
}