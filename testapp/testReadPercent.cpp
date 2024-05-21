#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

bool readFifoPipe(const std::string& fifoPath, std::string& buff) {
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

int main() {

    std::string percent;
    int temp;
    auto filepath = "/home/nuu9hc/FOTA/fotaStorage/fifoPercent";
    while (temp < 100) {
      if (readFifoPipe(filepath, percent)) {
        std::cout << "percent: " << percent << std::endl;
      }
        // temp = std::stoi(percent);
    }
    return 0;
}