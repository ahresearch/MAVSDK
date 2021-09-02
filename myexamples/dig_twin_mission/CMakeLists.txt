cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dig_twin_mission)

add_executable(dig_twin_mission
    dig_twin_mission.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(dig_twin_mission
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_mission_raw
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(dig_twin_mission PRIVATE -Wall -Wextra)
else()
    add_compile_options(dig_twin_mission PRIVATE -WX -W2)
endif()
