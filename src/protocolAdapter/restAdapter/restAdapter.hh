#include <string>

enum class CONNECT_CMD {
  CONNECT = 65,
  DISCONNECT = 66
};

enum class STATUSCODE {
  ERROR_OK = 65,
  FAILED = 66
};
class restAdapter {

  public:
    static bool readBinFileData(const std::string& file_path, std::string& bin_content);
    static void configConnection(const std::string& ip, std::string& urlUpdate, std::string& urlFileSize);
    static bool sendDataRequest(const std::string &url, const std::string &data);
    static bool sendFileRequest(const std::string &url, const std::string &file_path);
};