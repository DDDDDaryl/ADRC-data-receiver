cmake_minimum_required(VERSION 3.17)
project(receiver)

set(CMAKE_CXX_STANDARD 14)

add_executable(receiver main.cpp WzSerialPort.cpp receiver.cpp sender.cpp sender.h ref_sender.cpp ref_sender.h configure.cpp configure.h)