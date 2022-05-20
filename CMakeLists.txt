cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(LLShooter)

set(CMAKE_CXX_STANDARD 20)

add_executable(LLShooter
src/main.cpp
src/LLShooter.cpp
src/RenderingServer.cpp
src/FPSMeter.cpp
src/objects/SpatialObject.cpp
src/objects/DrawableObject.cpp
src/objects/MonochromeDrawableObject.cpp
src/objects/Camera.cpp
src/objects/ControllableCamera.cpp
src/utils/shader_loader.cpp
src/utils/model_loader.cpp
)

# GLFW.
find_package(glfw3 REQUIRED)
target_link_libraries(LLShooter glfw)

# GLEW.
find_package(GLEW REQUIRED)
target_link_libraries(LLShooter GLEW::GLEW)

# OpenGL.
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(LLShooter OpenGL)

# Copy resources to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/res")
