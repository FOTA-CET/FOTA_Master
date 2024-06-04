# FOTA_Master
- Monitor and coordinate the entire FOTA update process in the vehicle.
- Be notified by FOTA Connect when new updates are available.
- Retrieve updates from local storage and coordinate their delivery to target ECUs.
### Cross compile with conan:
create conan profile: 
```sh
[settings]
os=Linux
arch=armv7hf
compiler=gcc
build_type=Release
compiler.cppstd=gnu14
compiler.libcxx=libstdc++11
compiler.version=9
[buildenv]
CC=/home/nuu9hc/arm_compiler_2020/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc
CXX=/home/nuu9hc/arm_compiler_2020/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-g++
LD=/home/nuu9hc/arm_compiler_2020/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/arm-linux-gnueabihf-ld
```
### Build with conan
```sh
mkdir build
cd build
conan install .. --build=missing -pr=rasp3B
conan build .. -pr=rasp3B
```
### Design
## Sequence diagram
![image](https://github.com/FOTA-CET/FOTA_Master/assets/82272158/32961de3-b2af-44cc-b51c-9a1051946b95)
