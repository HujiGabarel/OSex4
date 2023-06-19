cmake_minimum_required(VERSION 3.21)
project(ex4)

set(CMAKE_CXX_STANDARD 14)

add_executable(ex4 SimpleTestNand.cpp PhysicalMemory.cpp PhysicalMemory.h VirtualMemory.h MemoryConstants.h VirtualMemory.cpp)
