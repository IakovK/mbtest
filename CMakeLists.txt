cmake_minimum_required(VERSION 3.0)

project(mbtest)
include(FindPkgConfig)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODBUS REQUIRED libmodbus)
include_directories(${MODBUS_INCLUDE_DIRS})

set(MBSERVER_SOURCES mbserver.cpp Worker.cpp ThreadDeleter.cpp ModbusServer.cpp ModbusConnection.cpp TcpServer.cpp TcpConnection.cpp BaseServer.cpp)
set(MBSERVER_HEADERS Worker.h ThreadDeleter.h ModbusServer.h ModbusConnection.h TcpServer.h TcpConnection.h BaseServer.h BlockingCollection.h protocol.h)
set(MBCLIENT_SOURCES mbclient.cpp)
set(MBCLIENT_HEADERS protocol.h)

add_executable(mbserver ${MBSERVER_SOURCES} ${MBSERVER_HEADERS})
add_executable(mbclient ${MBCLIENT_SOURCES} ${MBCLIENT_HEADERS})

target_link_libraries(
    mbserver
    ${MODBUS_LIBRARIES}
)

#install(TARGETS mbtest RUNTIME DESTINATION bin)
