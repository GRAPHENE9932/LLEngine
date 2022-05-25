cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(LLShooter)

set(CMAKE_CXX_STANDARD 20)

add_executable(LLShooter
src/main.cpp
src/LLShooter.cpp
src/RenderingServer.cpp
src/PhysicsServer.cpp
src/common/FPSMeter.cpp
src/common/Mesh.cpp
src/objects/SpatialObject.cpp
src/objects/DrawableObject.cpp
src/objects/MonochromeDrawableObject.cpp
src/objects/TexturedDrawableObject.cpp
src/objects/Camera.cpp
src/objects/ControllableCylinder.cpp
src/objects/FloorObject.cpp
src/objects/RectangularWall.cpp
src/objects/CuboidObject.cpp
src/objects/ImageObject2D.cpp
src/structs/Rect.cpp
src/structs/Cylinder.cpp
src/utils/shader_loader.cpp
src/utils/model_loader.cpp
src/utils/texture_loader.cpp
src/utils/vbo_indexer.cpp
src/utils/math.cpp
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
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/res")
