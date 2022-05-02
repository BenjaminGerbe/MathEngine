cmake_minimum_required(VERSION 3.21)
project(MathEngine)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
link_directories(${OPENGL_gl_LIBRARY})

include_directories(../libs/glfw/include)
include_directories(../libs/glew/include)

link_directories(../libs/glfw/lib-static-ucrt)
link_directories(../libs/glew/lib/Release/x64)

add_executable(MathEngine main.cpp ../common/GLShader.cpp)


target_link_libraries(MathEngine glfw3 ${OPENGL_gl_LIBRARY} glew32)

