set(SOURCES
    LLShooter.cpp
    RenderingServer.cpp
    BulletPhysicsServer.cpp
    common/Map.cpp
    common/FPSMeter.cpp
    common/core/Mesh.cpp
    common/core/GLTFLoading.cpp
    common/core/GLTFToNode.cpp
    common/core/Cubemap.cpp
    common/core/SceneFile.cpp
    common/core/GLFWWindow.cpp
    common/core/KTXTexture.cpp
    common/core/RGBETexture.cpp
    common/core/BitmapFont.cpp
    common/core/VkFormatInfo.cpp
    common/core/shaders/ShaderManager.cpp
    common/core/shaders/ColoredTextShader.cpp
    common/core/shaders/SkyboxShader.cpp
    common/core/shaders/CommonShader.cpp
    common/core/shaders/EquirectangularMapperShader.cpp
    nodes/core/rendering/DrawableNode.cpp
    nodes/core/rendering/CameraNode.cpp
    nodes/core/rendering/CommonDrawableNode.cpp
    nodes/core/rendering/PointLightNode.cpp
    nodes/core/rendering/SpotLight.cpp
    nodes/core/rendering/BitmapTextNode.cpp
    nodes/core/rendering/SpectatorCameraNode.cpp
    nodes/core/physics/bullet/BulletRigidBodyNode.cpp
    nodes/core/CompleteSpatialNode.cpp
    nodes/core/SpatialNode.cpp
    structs/shapes/Shape.cpp
    structs/shapes/SphereShape.cpp
    structs/shapes/BoxShape.cpp
    structs/shapes/CylinderShape.cpp
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
find_package(ZSTD REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib zstd)

# Bullet.
find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME}_lib ${BULLET_LIBRARIES})
