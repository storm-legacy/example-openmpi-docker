cmake_minimum_required(VERSION 3.18)
project(dockerOpenMpiExample)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package(MPI REQUIRED)
add_executable(application main.cpp)
target_link_libraries(application PUBLIC MPI::MPI_CXX)
