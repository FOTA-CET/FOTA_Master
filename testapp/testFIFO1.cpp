#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <iostream>

int main() {
    int fd1, fd2;
    std::string ecuName = "ESP32";
    std::string firmware = "ESP32_v1.0.bin";
    std::string fifoECU = "/home/nuu9hc/FOTA/fotaStorage/fifoECU";
    std::string fifoFlash = "/home/nuu9hc/FOTA/fotaStorage/fifoFirmware";

    mkfifo(fifoECU.c_str(), 0666);

    fd1 = open(fifoECU.c_str(), O_WRONLY);
    auto ret = write(fd1, ecuName.c_str(), sizeof(ecuName));
    std::cout << "ret = " << ret << std::endl;
    close(fd1);
    std::cout << "Successfile to write ecuName" << std::endl;

    mkfifo(fifoFlash.c_str(), 0666);
    fd2 = open(fifoFlash.c_str(), O_WRONLY);
    write(fd2, firmware.c_str(), sizeof(firmware));
    close(fd2);
    std::cout << "Successfile to write firmware" << std::endl;

    return 0;
}