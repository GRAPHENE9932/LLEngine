cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(LLEngine)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(src)
include_directories(include)
include_directories(include/LLEngine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (NOT DEFINED LLENGINE_BUILD_TESTS)
    set(LLENGINE_BUILD_TESTS 1)
endif()

if (NOT DEFINED LLENGINE_BUILD_DEMO)
    set(LLENGINE_BUILD_DEMO 1)
endif()

if (LLENGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tst)
endif()

if (LLENGINE_BUILD_DEMO)
    add_subdirectory(demo)
endif()

set(SOURCES
    src/rendering/GLFWWindow.cpp
    src/rendering/Mesh.cpp
    src/rendering/MeshPrimitives.cpp
    src/rendering/Skybox.cpp
    src/rendering/shaders/PBRShaderManager.cpp
    src/rendering/shaders/PBRShader.cpp
    src/rendering/shaders/GaussianBlurShader.cpp
    src/rendering/GLTFLoading.cpp
    src/rendering/RenderingServer.cpp
    src/rendering/GLTFToNode.cpp
    src/rendering/Texture.cpp
    src/rendering/TextureFromKTX.cpp
    src/rendering/TextureFromRGBE.cpp
    src/rendering/ManagedFramebufferID.cpp
    src/rendering/ManagedRenderbufferID.cpp
    src/rendering/MainFramebuffer.cpp
    src/rendering/Shader.cpp
    src/rendering/BloomRenderer.cpp
    src/rendering/BloomFramebuffer.cpp
    src/rendering/ShadowMap.cpp
    src/rendering/ExposureController.cpp
    src/rendering/LightingEnvironment.cpp
    src/physics/BulletPhysicsServer.cpp
    src/utils/shader_loader.cpp
    src/utils/texture_utils.cpp
    src/structs/shapes/SphereShape.cpp
    src/structs/shapes/CylinderShape.cpp
    src/structs/shapes/BoxShape.cpp
    src/structs/shapes/Shape.cpp
    src/gui/FreeTypeFont.cpp
    src/gui/GUITexture.cpp
    src/gui/GUITransform.cpp
    src/SceneFile.cpp
    src/SceneJSON.cpp
    src/CustomNodeType.cpp
    src/node_registration.cpp
    src/nodes/rendering/SpectatorCameraNode.cpp
    src/nodes/rendering/PBRDrawableNode.cpp
    src/nodes/rendering/DrawableCompleteSpatialNode.cpp
    src/nodes/rendering/PointLightNode.cpp
    src/nodes/rendering/CameraNode.cpp
    src/nodes/rendering/ExplosionParticlesNode.cpp
    src/nodes/physics/BulletRigidBodyNode.cpp
    src/nodes/SpatialNode.cpp
    src/nodes/gui/GUINode.cpp
    src/nodes/gui/CompleteGUINode.cpp
    src/nodes/gui/ButtonNode.cpp
    src/nodes/gui/TextNode.cpp
    src/nodes/gui/GUICanvas.cpp
    src/nodes/CompleteSpatialNode.cpp
    src/nodes/Node.cpp
    src/logger.cpp
    src/GameInstance.cpp
    src/NodeProperty.cpp
    src/SceneJSON.cpp
    src/node_registration.cpp
    src/builtin_node_registration.cpp
    src/random.cpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND SOURCES src/rendering/OpenGLDebug.cpp)
endif()

set(PUBLIC_HEADERS
    include/LLEngine/CustomNodeType.hpp
    include/LLEngine/Event.hpp
    include/LLEngine/GameInstance.hpp
    include/LLEngine/GameSettings.hpp
    include/LLEngine/Key.hpp
    include/LLEngine/NodeProperty.hpp
    include/LLEngine/SceneFile.hpp
    include/LLEngine/SceneJSON.hpp
    include/LLEngine/Transform.hpp
    include/LLEngine/datatypes.hpp
    include/LLEngine/gui/FreeTypeFont.hpp
    include/LLEngine/gui/GUITexture.hpp
    include/LLEngine/gui/GUITransform.hpp
    include/LLEngine/logger.hpp
    include/LLEngine/math/AABB.hpp
    include/LLEngine/math/Frustum.hpp
    include/LLEngine/math/Plane.hpp
    include/LLEngine/node_cast.hpp
    include/LLEngine/node_registration.hpp
    include/LLEngine/nodes/CompleteSpatialNode.hpp
    include/LLEngine/nodes/Node.hpp
    include/LLEngine/nodes/SpatialNode.hpp
    include/LLEngine/nodes/gui/ButtonNode.hpp
    include/LLEngine/nodes/gui/CompleteGUINode.hpp
    include/LLEngine/nodes/gui/GUICanvas.hpp
    include/LLEngine/nodes/gui/GUINode.hpp
    include/LLEngine/nodes/gui/TextNode.hpp
    include/LLEngine/nodes/physics/BulletRigidBodyNode.hpp
    include/LLEngine/nodes/rendering/CameraNode.hpp
    include/LLEngine/nodes/rendering/Drawable.hpp
    include/LLEngine/nodes/rendering/DrawableCompleteSpatialNode.hpp
    include/LLEngine/nodes/rendering/ExplosionParticlesNode.hpp
    include/LLEngine/nodes/rendering/PBRDrawableNode.hpp
    include/LLEngine/nodes/rendering/PointLightNode.hpp
    include/LLEngine/nodes/rendering/SpectatorCameraNode.hpp
    include/LLEngine/physics/BulletPhysicsServer.hpp
    include/LLEngine/physics/shapes/BoxShape.hpp
    include/LLEngine/physics/shapes/CylinderShape.hpp
    include/LLEngine/physics/shapes/Shape.hpp
    include/LLEngine/physics/shapes/SphereShape.hpp
    include/LLEngine/random.hpp
    include/LLEngine/rendering/LazyShader.hpp
    include/LLEngine/rendering/ManagedFramebufferID.hpp
    include/LLEngine/rendering/Material.hpp
    include/LLEngine/rendering/RenderingServer.hpp
    include/LLEngine/rendering/Shader.hpp
    include/LLEngine/rendering/ShadowMap.hpp
    include/LLEngine/rendering/Skybox.hpp
    include/LLEngine/rendering/Window.hpp
    include/LLEngine/rendering/GLFWWindow.hpp
    include/LLEngine/rendering/LightingEnvironment.hpp
    include/LLEngine/rendering/Texture.hpp
    include/LLEngine/rendering/Mesh.hpp
    include/LLEngine/GLTF.hpp
    include/LLEngine/QualitySettings.hpp
)

find_package(Threads)
add_library(llengine SHARED ${SOURCES})
set_target_properties(llengine PROPERTIES PUBLIC_HEADER PUBLIC_HEADERS)

#target_compile_options(llengine PUBLIC -fsanitize=address)
#target_link_options(llengine PUBLIC -fsanitize=address)

# Enable warnings.
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

include(GNUInstallDirs)

# GLFW.
find_package(glfw3 REQUIRED)
target_link_libraries(llengine glfw)

# GLEW.
find_package(GLEW REQUIRED)
target_link_libraries(llengine GLEW::GLEW)

# Bullet.
find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIR})
target_link_libraries(llengine ${BULLET_LIBRARIES})

# nlohmann-json.
find_package(nlohmann_json REQUIRED)
target_link_libraries(llengine nlohmann_json::nlohmann_json)

# glm.
find_package(glm REQUIRED)
target_link_libraries(llengine glm)

# fmtlib.
find_package(fmt REQUIRED)
target_link_libraries(llengine fmt)

# libktx.
find_package(Ktx REQUIRED)
target_link_libraries(llengine KTX::ktx)

# FreeType.
find_package(Freetype REQUIRED)
target_link_libraries(llengine Freetype::Freetype)