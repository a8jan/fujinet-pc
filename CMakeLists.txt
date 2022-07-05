# fujinet-pc
cmake_minimum_required(VERSION 3.7.2...3.22)
project(fujinet-pc)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# fujinet.build_platform
set(FUJINET_BUILD_PLATFORM BUILD_ATARI)
# fujinet.build_bus
set(FUJINET_BUILD_BUS SIO)
# platformio.data_dir
set(FUJINET_DATA_DIR ${CMAKE_SOURCE_DIR}/${FUJINET_BUILD_PLATFORM}/data)

# add -DNO_DEBUG_PRINT to supress Debug_print output in Release build
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${FUJINET_BUILD_PLATFORM} -DSKIP_SERVER_CERT_VERIFY")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D${FUJINET_BUILD_PLATFORM} -DSKIP_SERVER_CERT_VERIFY -DVERBOSE_HTTP -D__PC_BUILD_DEBUG__")

# mongoose.c some compile options: -DMG_ENABLE_LINES=1 -DMG_ENABLE_DIRECTORY_LISTING=1 -DMG_ENABLE_SSI=1
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${FUJINET_BUILD_PLATFORM} -DMG_ENABLE_OPENSSL=1 -DMG_ENABLE_LOG=0")

# INCLUDE (CheckIncludeFiles)
# CHECK_INCLUDE_FILES (bsd/string.h HAVE_BSD_STRING_H)
# CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)
# set(INCLUDE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}/include

set(INCLUDE_DIRS include
    lib/compat lib/config lib/utils lib/hardware
    lib/FileSystem lib/EdUrlParser
    lib/tcpip lib/ftp lib/TNFSlib lib/telnet lib/fnjson
    lib/mongoose lib/webdav lib/http lib/task
    lib/modem-sniffer lib/printer-emulator lib/network-protocol 
    lib/bus lib/device lib/media
    components/cJSON components/libsmb2/include
)

set(SOURCES src/main.cpp
    lib/config/fnConfig.h lib/config/fnConfig.cpp
    lib/utils/utils.h lib/utils/utils.cpp
    lib/utils/cbuf.h lib/utils/cbuf.cpp
    lib/hardware/fnDummyWiFi.h lib/hardware/fnDummyWiFi.cpp
    lib/hardware/fnUART.h lib/hardware/fnUART.cpp
    lib/hardware/fnSystem.h lib/hardware/fnSystem.cpp lib/hardware/fnSystemNet.cpp
    lib/FileSystem/fnFS.h lib/FileSystem/fnFS.cpp
    lib/FileSystem/fnFsSPIF.h lib/FileSystem/fnFsSPIF.cpp
    lib/FileSystem/fnFsSD.h lib/FileSystem/fnFsSD.cpp
    lib/FileSystem/fnFsTNFS.h lib/FileSystem/fnFsTNFS.cpp
    lib/FileSystem/fnFile.h lib/FileSystem/fnFile.cpp
    lib/FileSystem/fnFileLocal.h lib/FileSystem/fnFileLocal.cpp
    lib/FileSystem/fnFileTNFS.h lib/FileSystem/fnFileTNFS.cpp
    lib/EdUrlParser/EdUrlParser.h lib/EdUrlParser/EdUrlParser.cpp
    lib/tcpip/fnDNS.h lib/tcpip/fnDNS.cpp
    lib/tcpip/fnUDP.h lib/tcpip/fnUDP.cpp
    lib/tcpip/fnTcpClient.h lib/tcpip/fnTcpClient.cpp
    lib/tcpip/fnTcpServer.h lib/tcpip/fnTcpServer.cpp
    lib/ftp/fnFTP.h lib/ftp/fnFTP.cpp
    lib/TNFSlib/tnfslibMountInfo.h lib/TNFSlib/tnfslibMountInfo.cpp
    lib/TNFSlib/tnfslib.h lib/TNFSlib/tnfslib.cpp
    lib/telnet/libtelnet.h lib/telnet/libtelnet.c
    lib/fnjson/fnjson.h lib/fnjson/fnjson.cpp
    lib/mongoose/mongoose.h lib/mongoose/mongoose.c
    lib/webdav/WebDAV.h lib/webdav/WebDAV.cpp
    lib/http/httpService.h lib/http/httpService.cpp
    lib/http/httpServiceParser.h lib/http/httpServiceParser.cpp
    lib/http/httpServiceConfigurator.h lib/http/httpServiceConfigurator.cpp
    lib/http/httpServiceBrowser.h lib/http/httpServiceBrowser.cpp
    lib/http/mgHttpClient.h lib/http/mgHttpClient.cpp
    lib/task/fnTask.h lib/task/fnTask.cpp
    lib/task/fnTaskManager.h lib/task/fnTaskManager.cpp
    lib/modem-sniffer/modem-sniffer.h lib/modem-sniffer/modem-sniffer.cpp
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
    lib/network-protocol//networkStatus.h
    lib/network-protocol/Protocol.h lib/network-protocol/Protocol.cpp
    lib/network-protocol/Test.h lib/network-protocol/Test.cpp
    lib/network-protocol/TCP.h lib/network-protocol/TCP.cpp
    lib/network-protocol/UDP.h lib/network-protocol/UDP.cpp
    lib/network-protocol/Telnet.h lib/network-protocol/Telnet.cpp
    lib/network-protocol/FS.h lib/network-protocol/FS.cpp
    lib/network-protocol/FTP.h lib/network-protocol/FTP.cpp
    lib/network-protocol/TNFS.h lib/network-protocol/TNFS.cpp
    lib/network-protocol/HTTP.h lib/network-protocol/HTTP.cpp
    lib/fuji/fujiHost.h lib/fuji/fujiHost.cpp
    lib/fuji/fujiDisk.h lib/fuji/fujiDisk.cpp
    lib/bus/bus.h
    lib/bus/sio/sio.h lib/bus/sio/sio.cpp
    lib/bus/sio/siocom/sioport.h lib/bus/sio/siocom/sioport.cpp
    lib/bus/sio/siocom/serialsio.h lib/bus/sio/siocom/serialsio.cpp
    lib/bus/sio/siocom/netsio.h lib/bus/sio/siocom/netsio.cpp
    lib/bus/sio/siocom/fnSioCom.h lib/bus/sio/siocom/fnSioCom.cpp
    lib/device/device.h
    lib/device/disk.h
    lib/device/printer.h
    lib/device/modem.h
    lib/device/cassette.h
    lib/device/fuji.h
    lib/device/network.h
    lib/device/udpstream.h
    lib/device/siocpm.h
    lib/device/sio/disk.h lib/device/sio/disk.cpp
    lib/device/sio/printer.h lib/device/sio/printer.cpp
    lib/device/sio/printerlist.h lib/device/sio/printerlist.cpp
    lib/device/sio/modem.h lib/device/sio/modem.cpp
    lib/device/sio/cassette.h lib/device/sio/cassette.cpp
    lib/device/sio/fuji.h lib/device/sio/fuji.cpp
    lib/device/sio/network.h lib/device/sio/network.cpp
    lib/device/sio/apetime.h lib/device/sio/apetime.cpp
    lib/media/media.h
    lib/media/atari/diskType.h lib/media/atari/diskType.cpp
    lib/media/atari/diskTypeAtr.h lib/media/atari/diskTypeAtr.cpp
    lib/media/atari/diskTypeAtx.h 
    lib/media/atari/diskTypeXex.h lib/media/atari/diskTypeXex.cpp
    lib/compat/compat_inet.c
    lib/compat/compat_gettimeofday.c
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SOURCES ${SOURCES} lib/compat/win32_uname.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
# strlcpy and strlcat are part of stdlib on mac
else()
    set(SOURCES ${SOURCES} lib/compat/strlcat.c lib/compat/strlcpy.c)
endif()

# cJSON library
# https://github.com/DaveGamble/cJSON
set(ENABLE_CJSON_UTILS ON CACHE BOOL "Enable building the cJSON_Utils library.")
set(ENABLE_CJSON_TEST OFF CACHE BOOL "Enable building cJSON test")
set(CJSON_OVERRIDE_BUILD_SHARED_LIBS ON CACHE BOOL "Override BUILD_SHARED_LIBS with CJSON_BUILD_SHARED_LIBS")
set(CJSON_BUILD_SHARED_LIBS OFF CACHE BOOL "Overrides BUILD_SHARED_LIBS if CJSON_OVERRIDE_BUILD_SHARED_LIBS is enabled")
add_subdirectory(components/cJSON)
add_link_options("-fstack-protector")

add_custom_command(
  OUTPUT include/version.h
  COMMAND python build_version.py
  DEPENDS ${SOURCE_FILES} build_version.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(fujinet ${SOURCES} include/version.h)

set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
find_package(OpenSSL REQUIRED)

target_include_directories(fujinet PRIVATE ${INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(fujinet pthread expat OpenSSL::SSL OpenSSL::Crypto cjson cjson_utils)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(fujinet ws2_32)
endif()

# if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#     target_link_libraries(fujinet bsd)
# endif()

# "dist" target
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_target(dist
        COMMENT "Preparing dist directory"
        COMMAND ${CMAKE_COMMAND} -E make_directory dist
        # COMMAND echo Getting fujinet-sd-card
        # COMMAND ${CMAKE_COMMAND} -E remove_directory dist/SD
        # COMMAND git clone --depth 1 https://github.com/FujiNetWIFI/fujinet-sd-card.git dist/SD
        # COMMAND ${CMAKE_COMMAND} -E remove_directory dist/SD/.git
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/distfiles dist
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fujinet> dist
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${FUJINET_DATA_DIR} dist/data
        # DLL's TODO how to make this using cmake?
        COMMAND ldd $<TARGET_FILE:fujinet> | grep -v -i '/windows' 
        | awk '{print $$3}' | xargs -I {} cp -p {} dist
    )
else()
    add_custom_target(dist
        COMMENT "Preparing dist directory"
        COMMAND ${CMAKE_COMMAND} -E make_directory dist
        # COMMAND echo Getting fujinet-sd-card
        # COMMAND ${CMAKE_COMMAND} -E remove_directory dist/SD
        # COMMAND git clone --depth 1 https://github.com/FujiNetWIFI/fujinet-sd-card.git dist/SD
        # COMMAND ${CMAKE_COMMAND} -E remove_directory dist/SD/.git
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/distfiles dist
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:fujinet> dist
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${FUJINET_DATA_DIR} dist/data
        COMMAND ${CMAKE_COMMAND} -E remove dist/run-fujinet.bat
    )
endif()
add_dependencies(dist fujinet)

# include dist cleanup in "clean" target
set_property(
    DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES dist
)
