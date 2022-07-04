set(SOURCES
    LLShooter.cpp
    RenderingServer.cpp
    PhysicsServer.cpp
    common/FPSMeter.cpp
    common/Mesh.cpp
    common/Map.cpp
    common/KTXTexture.cpp
    common/BitmapFont.cpp
    common/VkFormatInfo.cpp
    common/shaders/ShaderManager.cpp
    common/shaders/ColoredTextShader.cpp
    common/shaders/SkyboxShader.cpp
    common/shaders/TexturedShader.cpp
    common/shaders/UnshadedShader.cpp
    common/shaders/UnshadedTexturedShader.cpp
    objects/SpatialObject.cpp
    objects/TexturedDrawableObject.cpp
    objects/UnshadedDrawableObject.cpp
    objects/RectangularPhysicsObject.cpp
    objects/Camera.cpp
    objects/ControllableCylinder.cpp
    objects/FloorObject.cpp
    objects/RectangularWall.cpp
    objects/CuboidObject.cpp
    objects/ImageObject.cpp
    objects/SkyboxObject.cpp
    objects/PointLight.cpp
    objects/SpotLight.cpp
    objects/BitmapTextObject.cpp
    structs/Circle.cpp
    structs/QuadrantArc.cpp
    structs/Rect.cpp
    structs/HorLS.cpp
    structs/VertLS.cpp
    structs/RoundedRectangle.cpp
    utils/shader_loader.cpp
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

# Enable warnings.
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
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

# Zstd.
find_package(zstd REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib zstd)
