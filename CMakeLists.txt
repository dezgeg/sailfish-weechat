cmake_minimum_required(VERSION 2.8.4)
project(sailfish-weechat)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O0 -gdwarf-2 -Wall -Wextra -Woverloaded-virtual -Werror -Wno-unused-parameter")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5XmlPatterns REQUIRED)

file(GLOB_RECURSE srcs src/**.cpp src/**.hpp)

add_executable(foo ${srcs})
qt5_use_modules(foo Widgets Multimedia OpenGL Qml Xml XmlPatterns)
