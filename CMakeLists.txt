# fujinet-platfromio
# cmake_minimum_required(VERSION 3.16.0)
# include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# project(FujiNet_idf)


# fujinet-open
cmake_minimum_required(VERSION 3.10)
project(fujinet-open VERSION 0.1.0)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__OPEN_BUILD_DEBUG__")
set(INCLUDE_DIRS include lib/config lib/FileSystem lib/utils lib/hardware lib/tcpip lib/telnet lib/sio)
set(SOURCES src/main.cpp
    lib/config/fnConfig.h lib/config/fnConfig.cpp
    lib/FileSystem/fnFS.h lib/FileSystem/fnFS.cpp
    lib/FileSystem/fnFsSPIF.h lib/FileSystem/fnFsSPIF.cpp
    lib/FileSystem/fnFsSD.h lib/FileSystem/fnFsSD.cpp
    # lib/FileSystem/fnFsTNFS.h lib/FileSystem/fnFsTNFS.cpp
    lib/utils/utils.h lib/utils/utils.cpp
    lib/utils/cbuf.h lib/utils/cbuf.cpp
    lib/hardware/fnUART.h lib/hardware/fnUART.cpp
    lib/hardware/fnSystem.h lib/hardware/fnSystem.cpp
    lib/sio/diskType.h lib/sio/diskType.cpp
    lib/sio/diskTypeAtr.h lib/sio/diskTypeAtr.cpp
    lib/sio/diskTypeAtx.h
    lib/sio/diskTypeXex.h lib/sio/diskTypeXex.cpp
    lib/tcpip/fnDNS.h lib/tcpip/fnDNS.cpp
    lib/tcpip/fnUDP.h lib/tcpip/fnUDP.cpp
    lib/tcpip/fnTcpClient.h lib/tcpip/fnTcpClient.cpp
    lib/tcpip/fnTcpServer.h lib/tcpip/fnTcpServer.cpp
    lib/TNFSlib/tnfslibMountInfo.h lib/TNFSlib/tnfslibMountInfo.cpp
    lib/TNFSlib/tnfslib.h lib/TNFSlib/tnfslib.cpp
    lib/sio/printer.h
    lib/modem-sniffer/modem-sniffer.h lib/modem-sniffer/modem-sniffer.cpp
    lib/telnet/libtelnet.h lib/telnet/libtelnet.c
    lib/sio/modem.h lib/sio/modem.cpp
    lib/sio/sio.h lib/sio/sio.cpp
    lib/sio/disk.h lib/sio/disk.cpp
    lib/sio/fujiHost.h lib/sio/fujiHost.cpp
    lib/sio/fujiDisk.h lib/sio/fujiDisk.cpp
    lib/sio/fuji.h lib/sio/fuji.cpp
)
add_executable(fujinet ${SOURCES})

target_include_directories(fujinet PRIVATE ${INCLUDE_DIRS})
target_link_libraries(fujinet bsd)
