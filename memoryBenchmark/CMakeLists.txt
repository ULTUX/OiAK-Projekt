cmake_minimum_required(VERSION 3.0.0)
project(testingOiAK VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(testingOiAK main.cpp CSVWriter.cpp)

set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(CPack)
