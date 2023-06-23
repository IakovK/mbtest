cmake_minimum_required(VERSION 3.0)

project(mbtest)
include(FindPkgConfig)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODBUS REQUIRED libmodbus)
include_directories(${MODBUS_INCLUDE_DIRS})
add_executable(mbserver mbserver.cpp Worker.cpp ThreadDeleter.cpp ModbusServer.cpp ModbusConnection.cpp Worker.h ThreadDeleter.h ModbusServer.h ModbusConnection.h BlockingCollection.h)

target_link_libraries(
    mbserver
    ${MODBUS_LIBRARIES}
)

#install(TARGETS mbtest RUNTIME DESTINATION bin)
