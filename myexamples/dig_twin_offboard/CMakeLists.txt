cmake_minimum_required(VERSION 2.8.12)

project(dig_twin_offboard)

find_package(Threads REQUIRED)

if(MINGW)
    add_definitions("-D_USE_MATH_DEFINES") # For M_PI
endif()

if(MSVC)
    add_definitions("-std=c++11 -WX -W2")
    add_definitions("-D_USE_MATH_DEFINES") # For M_PI
else()
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
endif()

find_package(MAVSDK REQUIRED)

add_executable(dig_twin_offboard
    dig_twin_offboard.cpp
)

target_link_libraries(dig_twin_offboard
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_offboard
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
    ${CMAKE_THREAD_LIBS_INIT}
)
