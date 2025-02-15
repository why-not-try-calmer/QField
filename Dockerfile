FROM ghcr.io/opengisch/qgis-slim:latest

ENV DEBIAN_FRONTEND noninteractive
ENV TZ Etc/UTC

# System utilities for Debian 11 Buster
RUN apt update -y && apt install gnupg2 wget software-properties-common -y

# Build tools (cmake >= 3.19 required)
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add - && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt update -y && \
    apt install curl zip unzip tar make cmake cmake-curses-gui g++ gcc ninja-build build-essential bison -y

# Qt dependencies
RUN apt install libgl1-mesa-dev libgstreamer-gl1.0-0 libpulse-dev libxcb-glx0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-render0 libxcb-shape0 libxcb-shm0 libxcb-sync1 libxcb-util1 libxcb-xfixes0 libxcb-xinerama0 libxcb1 libxkbcommon-dev libxkbcommon-x11-0 libxcb-xkb-dev -y

# Qt
RUN apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools -y

# Not needed when wuing VCPKG: Qgis dependencies
RUN apt install dh-python doxygen expect flex flip gdal-bin git graphviz grass-dev libexiv2-dev libexpat1-dev libfcgi-dev libgdal-dev libgeos-dev libgsl-dev libpdal-dev libpq-dev libproj-dev libprotobuf-dev libqca-qt5-2-dev libqca-qt5-2-plugins libqscintilla2-qt5-dev libqt5opengl5-dev libqt5serialport5-dev libqt5sql5-sqlite libqt5svg5-dev libqt5webkit5-dev libqt5xmlpatterns5-dev libqwt-qt5-dev libspatialindex-dev libspatialite-dev libsqlite3-dev libsqlite3-mod-spatialite libyaml-tiny-perl libzip-dev libzstd-dev lighttpd locales ninja-build ocl-icd-opencl-dev opencl-headers pandoc pdal pkg-config poppler-utils protobuf-compiler pyqt5-dev pyqt5-dev-tools pyqt5.qsci-dev python3-all-dev python3-autopep8 python3-dateutil python3-dev python3-future python3-gdal python3-httplib2 python3-jinja2 python3-lxml python3-markupsafe python3-mock python3-nose2 python3-owslib python3-plotly python3-psycopg2 python3-pygments python3-pyproj python3-pyqt5 python3-pyqt5.qsci python3-pyqt5.qtpositioning python3-pyqt5.qtsql python3-pyqt5.qtsvg python3-pyqt5.qtwebkit python3-requests python3-sip python3-sip-dev python3-termcolor python3-tz python3-yaml qt3d-assimpsceneimport-plugin qt3d-defaultgeometryloader-plugin qt3d-gltfsceneio-plugin qt3d-scene2d-plugin qt3d5-dev qt5keychain-dev qtbase5-dev qtbase5-private-dev qtpositioning5-dev qttools5-dev qttools5-dev-tools spawn-fcgi xauth xfonts-100dpi xfonts-75dpi xfonts-base xfonts-scalable xvfb -y

# Not needed when wuing VCPKG: QField dependencies
RUN apt install libqt5sensors5-dev libqt5webview5-dev libqt5multimedia5-plugins libqt5multimedia5 qtmultimedia5-dev libzxingcore-dev  libqt5bluetooth5 qtconnectivity5-dev qml-module-qtbluetooth qml-module-qtlocation qml-module-qtwebengine qml-module-qtgraphicaleffects qml-module-qt-labs-settings qml-module-qtquick-controls2 qml-module-qtquick-layouts qml-module-qtwebview qml-module-qtmultimedia qml-module-qtquick-shapes qml-module-qtsensors qml-module-qt-labs-calendar qml-module-qtquick-particles2  gperf autopoint '^libxcb.*-dev' libx11-xcb-dev libegl1-mesa libegl1-mesa-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev autoconf-archive libgstreamer-plugins-base1.0-dev -y

WORKDIR /opt/builder
COPY . .

CMD ["./entrypoint.sh"]
