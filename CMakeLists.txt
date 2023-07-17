cmake_minimum_required(VERSION 3.0.0)
project(ReadLine VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(ReadLine rdlines.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
