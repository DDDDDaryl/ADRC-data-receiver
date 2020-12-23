cmake_minimum_required(VERSION 3.17)
project(receiver)

SET(CMAKE_CXX_STANDARD 14)
SET(PROJECT_ROOT_PATH "${PROJECT_SOURCE_DIR}/../")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib/")

add_executable(receiver main.cpp WzSerialPort.cpp receiver.cpp sender.cpp sender.h ref_sender.cpp ref_sender.h configure.cpp configure.h)