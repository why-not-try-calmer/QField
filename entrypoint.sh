#!/bin/bash
echo "Press 'b' to configure and build, 'c' to compile and link, or 'a' to abort."
read -e choice
case "$choice" in
    "b" ) cmake -S . -B build -G Ninja -D CMAKE_CXX_COMPILER=/usr/bin/g++ -D CMAKE_C_COMPILER=/usr/bin/gcc -D CMAKE_MAKE_PROGRAM=/usr/bin/ninja -D CMAKE_INSTALL_LIBDIR=/lib -D CMAKE_BUILD_DIR=/opt/builder/build -D CMAKE_BUILD_TYPE=Release -D WITH_VCPKG=ON -D SYSTEM_QT=ON -D CMAKE_INSTALL_PREFIX=/usr ;;
    "c" ) cmake --build build --config Release ;;
    "a" ) exit 0;;
esac