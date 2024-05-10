#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h" 
#include "rapidjson/writer.h"
#include <errno.h>
#include <iostream>
#include <string>

#include <ecu_config.hh>

using namespace rapidjson;

std::vector<ecuInfo> EcuConfig::ecuInfoList;

void EcuConfig::Parse(const std::string filePath) {
  FILE* fp = fopen(filePath.c_str(), "r"); 
  char readBuffer[65536];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document doc;
  doc.ParseStream(is);
  fclose(fp);
  
  std::string name;
  std::string canInterface;
  std::string can_id_size;
  std::string can_id_Fimware;
  std::string ip;

  if(doc.HasMember("ECU") && doc["ECU"].IsArray()) {
    const Value& ECUs = doc["ECU"];
    for(auto i = 0; i <ECUs.Size(); i++) {
      const Value& ecu = ECUs[i];
      if(ecu.HasMember("name") && ecu["name"].IsString()) {
        name = ecu["name"].GetString();
        if(ecu.HasMember("canInterface") && ecu["canInterface"].IsString()) {
          canInterface = ecu["canInterface"].GetString();
        } else {
          throw std::runtime_error("EcuConfig::Parse eci.config does not contain canInterface");
        }
        if(ecu.HasMember("can_id_firmware") && ecu["can_id_firmware"].IsString()) {
          can_id_Fimware = ecu["can_id_firmware"].GetString();
        } else {
          throw std::runtime_error("EcuConfig::Parse eci.config does not contain can_id_Fimware");
        }
        if (name == "ESP32") {
          if(ecu.HasMember("ip") && ecu["ip"].IsString()) {
            ip = ecu["ip"].GetString();
          } else {
            throw std::runtime_error("EcuConfig::Parse eci.config ESP32 does not contain ip");
          }
        } else {
          if(ecu.HasMember("can_id_size") && ecu["can_id_size"].IsString()) {
            can_id_size = ecu["can_id_size"].GetString();
          } else {
            throw std::runtime_error("EcuConfig::Parse eci.config does not contain can_id_size");
          }
        }
      } else {
        throw std::runtime_error("EcuConfig::Parse eci.config does not contain name");
      }
      ecuInfo ecuInfor {name, canInterface, can_id_size, can_id_Fimware, ip};
      ecuInfoList.push_back(ecuInfor);
    }
  }

}

ecuInfo EcuConfig::getEcuInfo(std::string name) {
  for(auto x : ecuInfoList) {
    if (name == x.name) return x;
  }
}
