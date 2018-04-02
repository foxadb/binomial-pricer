# Source files
file (GLOB LIB_HEADERS *.hpp)
file (GLOB LIB_SOURCES *.cpp)
add_library (binomial-pricer ${LIB_HEADERS} ${LIB_SOURCES})

# Copy kernel file
configure_file (kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl COPYONLY)
