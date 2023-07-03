#!/bin/bash
echo "Press 'c' to configure and build dependencies, 'b' to build the application, 'cb' to configure and build the application, or 'e' to exit."
read -e choice
case "$choice" in
    "c" ) cmake -S . -B build -G Ninja -D CMAKE_CXX_COMPILER=/usr/bin/g++ -D CMAKE_C_COMPILER=/usr/bin/gcc -D CMAKE_MAKE_PROGRAM=/usr/bin/ninja -D CMAKE_INSTALL_LIBDIR=/lib -D CMAKE_BUILD_DIR=/opt/builder/build -D CMAKE_BUILD_TYPE=Release -D WITH_VCPKG=ON -D SYSTEM_QT=ON -D CMAKE_INSTALL_PREFIX=/usr ;;
    "b" ) cmake --build build --config Release ;;
    "cb") 
        cmake -S . -B build -G Ninja -D CMAKE_CXX_COMPILER=/usr/bin/g++ -D CMAKE_C_COMPILER=/usr/bin/gcc -D CMAKE_MAKE_PROGRAM=/usr/bin/ninja -D CMAKE_INSTALL_LIBDIR=/lib -D CMAKE_BUILD_DIR=/opt/builder/build -D CMAKE_BUILD_TYPE=Release -D WITH_VCPKG=ON -D SYSTEM_QT=ON -D CMAKE_INSTALL_PREFIX=/usr;
        cmake --build build --config Release;;
    "a" ) exit 0;;
esac