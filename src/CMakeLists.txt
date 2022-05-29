set(SOURCES
    LLShooter.cpp
    RenderingServer.cpp
    PhysicsServer.cpp
    common/FPSMeter.cpp
    common/Mesh.cpp
    common/Map.cpp
    common/Texture.cpp
    objects/SpatialObject.cpp
    objects/DrawableObject.cpp
    objects/TexturedDrawableObject.cpp
    objects/UnshadedDrawableObject.cpp
    objects/RectangularPhysicsObject.cpp
    objects/Camera.cpp
    objects/ControllableCylinder.cpp
    objects/FloorObject.cpp
    objects/RectangularWall.cpp
    objects/CuboidObject.cpp
    objects/ImageObject2D.cpp
    objects/PointLight.cpp
    structs/Rect.cpp
    structs/Cylinder.cpp
    utils/shader_loader.cpp
    utils/model_loader.cpp
    utils/vbo_indexer.cpp
    utils/math.cpp
)

add_executable(${CMAKE_PROJECT_NAME}_run main.cpp)
add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME}_run ${CMAKE_PROJECT_NAME}_lib)

# Try to enable link time optimization.
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)
if (IPO_SUPPORTED)
    message(STATUS "IPO/LTO supported and enabled")
    set_property(TARGET ${CMAKE_PROJECT_NAME}_run PROPERTY INTERPROCERUDAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO/LTO is not supported and disabled")
endif()

# GLFW.
find_package(glfw3 REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib glfw)

# GLEW.
find_package(GLEW REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib GLEW::GLEW)

# OpenGL.
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib OpenGL)
