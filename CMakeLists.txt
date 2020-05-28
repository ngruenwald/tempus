cmake_minimum_required(VERSION 3.0)

project(
  tempus
  VERSION 0.1.0
  DESCRIPTION "timer stuff"
  LANGUAGES C CXX
)

#
# Options
#

option(
  TOOLS
  "Build the tools."
  OFF
)

option(
  UNIT_TESTS
  "Build the tests."
  OFF
)

option(
  TOOLS
  "build the tools"
  ON
)

#
# Basic compiler settings
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D -D_WIN32_WINNT=0x0601")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

#
#
#

if(UNIT_TESTS)
  enable_testing()
endif()

#
# Code
#

add_subdirectory(src)

#
# Documentation
#

add_subdirectory(doc)
