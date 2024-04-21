#include <vector>
#include "string"

#include "linux/can.h"

class canAdapter {
    public:
        static bool readHexFileData(const std::string& file_path, std::vector<uint8_t>& dataArray);
        static bool config(int& socket_fd, const std::string& canInterface);
        static bool checkConnect();
        static bool sendData(int& socket_fd, const can_frame& data_frame);
};