CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(binomial-pricer CXX)

# Handle OpenCL
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

# Add source directory
add_subdirectory(src)

# Generate executables from src
add_executable(cpu-pricer src/cpu-pricer.cpp)
target_link_libraries(cpu-pricer ${OpenCL_LIBRARY} binomial-pricer)

add_executable(gpu-pricer src/gpu-pricer.cpp)
target_link_libraries(gpu-pricer ${OpenCL_LIBRARY} binomial-pricer)
