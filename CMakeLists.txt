# fujinet-platfromio
# cmake_minimum_required(VERSION 3.16.0)
# include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# project(FujiNet_idf)


# fujinet-pc
cmake_minimum_required(VERSION 3.10)
project(fujinet-pc VERSION 0.1.0)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -D__PC_BUILD_DEBUG__ -DVERBOSE_TNFS -DVERBOSE_SIO")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -D__PC_BUILD_DEBUG__ -DSKIP_SERVER_CERT_VERIFY -DVERBOSE_HTTP")

# mongoose.c some compile options: -DMG_ENABLE_LINES=1 -DMG_ENABLE_DIRECTORY_LISTING=1 -DMG_ENABLE_SSI=1
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DMG_ENABLE_DIRECTORY_LISTING=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMG_ENABLE_OPENSSL=1 -DMG_ENABLE_LOG=0")

INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES (bsd/string.h HAVE_BSD_STRING_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

set(INCLUDE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}/include lib/config lib/FileSystem lib/utils lib/hardware 
    lib/tcpip lib/telnet  lib/EdUrlParser lib/TNFSlib
    lib/siocom lib/sio lib/printer-emulator lib/mongoose lib/http lib/task
    components/libsmb2/include)

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
    lib/hardware/fnSystem.h lib/hardware/fnSystem.cpp lib/hardware/fnSystemNet.cpp
    lib/sio/diskType.h lib/sio/diskType.cpp
    lib/sio/diskTypeAtr.h lib/sio/diskTypeAtr.cpp
    lib/sio/diskTypeAtx.h 
    lib/sio/diskTypeXex.h lib/sio/diskTypeXex.cpp
    lib/tcpip/fnDNS.h lib/tcpip/fnDNS.cpp
    lib/tcpip/fnUDP.h lib/tcpip/fnUDP.cpp
    lib/tcpip/fnTcpClient.h lib/tcpip/fnTcpClient.cpp
    lib/tcpip/fnTcpServer.h lib/tcpip/fnTcpServer.cpp
    lib/ftp/fnFTP.h lib/ftp/fnFTP.cpp
    lib/TNFSlib/tnfslibMountInfo.h lib/TNFSlib/tnfslibMountInfo.cpp
    lib/TNFSlib/tnfslib.h lib/TNFSlib/tnfslib.cpp
    lib/printer-emulator/atari_1020.h lib/printer-emulator/atari_1020.cpp
    lib/printer-emulator/atari_1025.h lib/printer-emulator/atari_1025.cpp
    lib/printer-emulator/atari_1027.h lib/printer-emulator/atari_1027.cpp
    lib/printer-emulator/atari_1029.h lib/printer-emulator/atari_1029.cpp
    lib/printer-emulator/atari_820.h lib/printer-emulator/atari_820.cpp
    lib/printer-emulator/atari_822.h lib/printer-emulator/atari_822.cpp
    lib/printer-emulator/atari_825.h lib/printer-emulator/atari_825.cpp
    lib/printer-emulator/atari_xdm121.h lib/printer-emulator/atari_xdm121.cpp
    lib/printer-emulator/atari_xmm801.h lib/printer-emulator/atari_xmm801.cpp
    lib/printer-emulator/epson_80.h lib/printer-emulator/epson_80.cpp
    lib/printer-emulator/epson_tps.h
    lib/printer-emulator/file_printer.h lib/printer-emulator/file_printer.cpp
    lib/printer-emulator/html_printer.h lib/printer-emulator/html_printer.cpp
    lib/printer-emulator/okimate_10.h lib/printer-emulator/okimate_10.cpp
    lib/printer-emulator/pdf_printer.h lib/printer-emulator/pdf_printer.cpp
    lib/printer-emulator/png_printer.h lib/printer-emulator/png_printer.cpp
    lib/printer-emulator/printer_emulator.h lib/printer-emulator/printer_emulator.cpp
    lib/printer-emulator/svg_plotter.h lib/printer-emulator/svg_plotter.cpp
    lib/sio/printerlist.h lib/sio/printerlist.cpp
    lib/sio/printer.h lib/sio/printer.cpp
    lib/telnet/libtelnet.h lib/telnet/libtelnet.c
    lib/modem-sniffer/modem-sniffer.h lib/modem-sniffer/modem-sniffer.cpp
    lib/sio/modem.h lib/sio/modem.cpp
    lib/sio/cassette.h lib/sio/cassette.cpp
    lib/sio/apetime.h lib/sio/apetime.cpp
    lib/sio/disk.h lib/sio/disk.cpp
    lib/sio/fujiHost.h lib/sio/fujiHost.cpp
    lib/sio/fujiDisk.h lib/sio/fujiDisk.cpp
    lib/sio/fuji.h lib/sio/fuji.cpp
    lib/EdUrlParser/EdUrlParser.h lib/EdUrlParser/EdUrlParser.cpp
    lib/network-protocol/Protocol.h lib/network-protocol/Protocol.cpp
    lib/network-protocol/Test.h lib/network-protocol/Test.cpp
    lib/network-protocol/TCP.h lib/network-protocol/TCP.cpp
    lib/network-protocol/UDP.h lib/network-protocol/UDP.cpp
    lib/network-protocol/Telnet.h lib/network-protocol/Telnet.cpp
    lib/network-protocol/FS.h lib/network-protocol/FS.cpp
    lib/network-protocol/FTP.h lib/network-protocol/FTP.cpp
    lib/network-protocol/TNFS.h lib/network-protocol/TNFS.cpp
    lib/network-protocol/HTTP.h lib/network-protocol/HTTP.cpp
    lib/siocom/sioport.h lib/siocom/sioport.cpp
    lib/siocom/serialsio.h lib/siocom/serialsio.cpp
    lib/siocom/netsio.h lib/siocom/netsio.cpp
    lib/siocom/fnSioCom.h lib/siocom/fnSioCom.cpp
    lib/sio/networkStatus.h lib/sio/network.h lib/sio/network.cpp
    lib/sio/sio.h lib/sio/sio.cpp
    lib/mongoose/mongoose.h lib/mongoose/mongoose.c
    lib/webdav/WebDAV.h lib/webdav/WebDAV.cpp
    lib/http/httpService.h lib/http/httpService.cpp
    lib/http/httpServiceParser.h lib/http/httpServiceParser.cpp
    lib/http/httpServiceConfigurator.h lib/http/httpServiceConfigurator.cpp
    lib/http/httpServiceBrowser.h lib/http/httpServiceBrowser.cpp
    lib/http/mgHttpClient.h lib/http/mgHttpClient.cpp
    lib/task/fnTask.h lib/task/fnTask.cpp
    lib/task/fnTaskManager.h lib/task/fnTaskManager.cpp
)

add_executable(fujinet ${SOURCES})

set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
find_package(OpenSSL REQUIRED)

target_include_directories(fujinet PRIVATE ${INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(fujinet pthread expat OpenSSL::SSL OpenSSL::Crypto)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(fujinet bsd)
endif()
