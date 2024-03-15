cmake_minimum_required(VERSION 3.0)
project(MySystemCProject)

# Set include directories
include_directories(
  "${PROJECT_SOURCE_DIR}/include"
  "${SystemC_INSTALL_DIR}/include"
)

# Add executable
add_executable(my_systemc_executable src/main.cpp)

# Link SystemC libraries
target_link_libraries(my_systemc_executable systemc)
