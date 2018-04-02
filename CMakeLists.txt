CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(binomial-pricer CXX)

# Handle OpenCL
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

# Add source directory
add_subdirectory(src)

# Generate executables from src
add_executable(standard-pricer src/standard-pricer.cpp)
target_link_libraries(standard-pricer ${OpenCL_LIBRARY} binomial-pricer)

add_executable(opencl-pricer src/opencl-pricer.cpp)
target_link_libraries(opencl-pricer ${OpenCL_LIBRARY} binomial-pricer)
