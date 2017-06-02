#cmake -DCMAKE_MODULE_PATH="<path to share/tulip in Tulip install directory>;<path to Infiniband directory>" -DRE2_INCLUDE_DIR=<path to re2 directory> -DRE2_LIBRARY=<path to libre2.so> -DCMAKE_BUILD_TYPE=Release <path to Infiniband directory>
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(IBAUTIL_DIR ${PROJECT_SOURCE_DIR})
FIND_PACKAGE(IBAUTIL REQUIRED)

add_definitions(-Wall -std=c++0x -fPIC)

SET(RE2_DIR ${PROJECT_SOURCE_DIR})
FIND_PACKAGE(RE2 REQUIRED)
INCLUDE_DIRECTORIES(${RE2_INCLUDE_DIR})

ADD_SUBDIRECTORY(plugins)
