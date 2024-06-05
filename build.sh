#!/usr/bin/bash

SOURCE=$(pwd)

echo "Building FOTA MASTER"
cd build
rm -rf *
conan install .. --build=missing -pr=rasp3B
conan build .. -pr=rasp3B
echo "Build successfully"

#copy executable file to share folder
# echo "copy executable file to share folder"
# cd /media/sf_ShareFolder

# if [ -f "FotaMaster" ]; then
#     rm FotaMaster
#     echo "Removed old FotaMaster"
# fi

# cd "$SOURCE/build/Release"
# cp FotaMaster /media/sf_ShareFolder
# echo "copy succesfully"
