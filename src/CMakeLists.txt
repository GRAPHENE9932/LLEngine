set(SOURCES
    LLShooter.cpp
    RenderingServer.cpp
    PhysicsServer.cpp
    SceneTree.cpp
    common/FPSMeter.cpp
    common/Mesh.cpp
    common/Map.cpp
    common/GLTF.cpp
    common/KTXTexture.cpp
    common/BitmapFont.cpp
    common/VkFormatInfo.cpp
    common/shaders/ShaderManager.cpp
    common/shaders/ColoredTextShader.cpp
    common/shaders/SkyboxShader.cpp
    common/shaders/CommonShader.cpp
    nodes/core/physics/RectangularPhysicsObject.cpp
    nodes/core/physics/ControllableCylinder.cpp
    nodes/core/physics/RectangularWall.cpp
    nodes/core/physics/CuboidObject.cpp
    nodes/core/rendering/CameraNode.cpp
    nodes/core/rendering/ImageNode.cpp
    nodes/core/rendering/CommonDrawableNode.cpp
    nodes/core/rendering/PointLightNode.cpp
    nodes/core/rendering/SpotLight.cpp
    nodes/core/rendering/BitmapTextNode.cpp
    nodes/core/rendering/SpectatorCameraNode.cpp
    nodes/core/SpatialNode.cpp
    common/Skybox.cpp
    structs/Circle.cpp
    structs/QuadrantArc.cpp
    structs/HorLS.cpp
    structs/VertLS.cpp
    structs/RoundedRectangle.cpp
    utils/shader_loader.cpp
)

add_executable(${CMAKE_PROJECT_NAME}_run main.cpp)
add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME}_run ${CMAKE_PROJECT_NAME}_lib)

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
