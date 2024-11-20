cmake_minimum_required(VERSION 3.10)
project(AmusementPark VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the executable and include all source files
add_executable(executable
    libtarga.c
    CubicBspline.cpp
    GenericException.cpp
    Ground.cpp
    Track.cpp
    Tree.cpp
    World.cpp
    WorldWindow.cpp
    vboindexer.cpp
)

# Add include directories
target_include_directories(executable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
target_link_libraries(executable
    fltk
    fltk_gl
    GLEW
    GL
    GLU
)

# copy .tga files to build directory
file(GLOB TGA_FILES "*.tga")
file(COPY ${TGA_FILES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
# copy .obj files to build directory
file(GLOB OBJ_FILES "*.obj")
file(COPY ${OBJ_FILES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
