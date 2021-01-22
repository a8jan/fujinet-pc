# fujinet-platfromio
# cmake_minimum_required(VERSION 3.16.0)
# include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# project(FujiNet_idf)


# fujinet-open
cmake_minimum_required(VERSION 3.10)
project(fujinet-open VERSION 0.1.0)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -D__OPEN_BUILD_DEBUG__ -DVERBOSE_TNFS")
# mongoose.c some compile options: -DMG_ENABLE_LINES=1 -DMG_ENABLE_DIRECTORY_LISTING=1 -DMG_ENABLE_SSI=1
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DMG_ENABLE_DIRECTORY_LISTING=1")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMG_ENABLE_DIRECTORY_LISTING=1")

set(INCLUDE_DIRS include lib/config lib/FileSystem lib/utils lib/hardware lib/tcpip lib/telnet lib/sio lib/mongoose lib/http)
set(SOURCES src/main.cpp
    lib/config/fnConfig.h lib/config/fnConfig.cpp
    lib/FileSystem/fnFS.h lib/FileSystem/fnFS.cpp
    lib/FileSystem/fnFsSPIF.h lib/FileSystem/fnFsSPIF.cpp
    lib/FileSystem/fnFsSD.h lib/FileSystem/fnFsSD.cpp
    lib/FileSystem/fnFsTNFS.h lib/FileSystem/fnFsTNFS.cpp
    lib//FileSystem/fnFile.h lib//FileSystem/fnFile.cpp
    lib//FileSystem/fnFileLocal.h lib//FileSystem/fnFileLocal.cpp
    lib//FileSystem/fnFileTNFS.h lib//FileSystem/fnFileTNFS.cpp
    lib/utils/utils.h lib/utils/utils.cpp
    lib/utils/cbuf.h lib/utils/cbuf.cpp
    lib/hardware/fnDummyWiFi.h lib/hardware/fnDummyWiFi.cpp
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
    lib/telnet/libtelnet.h lib/telnet/libtelnet.c
    lib/modem-sniffer/modem-sniffer.h lib/modem-sniffer/modem-sniffer.cpp
    lib/sio/modem.h lib/sio/modem.cpp
    lib/sio/apetime.h lib/sio/apetime.cpp
    lib/sio/disk.h lib/sio/disk.cpp
    lib/sio/fujiHost.h lib/sio/fujiHost.cpp
    lib/sio/fujiDisk.h lib/sio/fujiDisk.cpp
    lib/sio/fuji.h lib/sio/fuji.cpp
    lib/sio/sio.h lib/sio/sio.cpp
    lib/mongoose/mongoose.h lib/mongoose/mongoose.c
    lib/http/httpService.h lib/http/httpService.cpp
    lib/http/httpServiceParser.h lib/http/httpServiceParser.cpp
    lib/http/httpServiceConfigurator.h lib/http/httpServiceConfigurator.cpp
)
add_executable(fujinet ${SOURCES})

target_include_directories(fujinet PRIVATE ${INCLUDE_DIRS})
target_link_libraries(fujinet bsd)
