cmake_minimum_required(VERSION 3.10)
project(AmusementPark VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB C_SRCS "${SRC_DIR}/*.c")
file(GLOB CPP_SRCS "${SRC_DIR}/*.cpp")

file(GLOB MODELS "${CMAKE_CURRENT_SOURCE_DIR}/models/*.obj")
file(GLOB TEXTURES "${CMAKE_CURRENT_SOURCE_DIR}/textures/*.tga")

# Add the executable and include all source files
add_executable(executable ${C_SRCS} ${CPP_SRCS})

# Add include directories
target_include_directories(executable PUBLIC ${SRC_DIR}/include)

# Link libraries
target_link_libraries(executable
    fltk
    fltk_gl
    GLEW
    GL
    GLU
)

# copy models files to build directory
file(COPY ${MODELS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# copy textures files to build directory
file(COPY ${TEXTURES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
