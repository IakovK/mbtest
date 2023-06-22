cmake_minimum_required(VERSION 3.0)

project(mbtest)
include(FindPkgConfig)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODBUS REQUIRED libmodbus)
include_directories(${MODBUS_INCLUDE_DIRS})
add_executable(mbserver mbserver.cpp BlockingCollection.h)

target_link_libraries(
    mbserver
    ${MODBUS_LIBRARIES}
)

#install(TARGETS mbtest RUNTIME DESTINATION bin)
