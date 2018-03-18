# Handle OpenCL
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

# Source files
file(GLOB LIB_HEADERS *.hpp)
file(GLOB LIB_SOURCES *.cpp)
add_library(binomial-pricer ${LIB_HEADERS} ${LIB_SOURCES})

add_executable(pricer main.cpp)
target_link_libraries(pricer ${OpenCL_LIBRARY} binomial-pricer)
