cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(LLEngine)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)
include_directories(include)
include_directories(include/LLEngine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
    src/rendering/VkFormatInfo.cpp
    src/rendering/GLFWWindow.cpp
    src/rendering/Mesh.cpp
    src/rendering/MeshPrimitives.cpp
    src/rendering/Skybox.cpp
    src/rendering/shaders/EquirectangularMapperShader.hpp
    src/rendering/shaders/IrradiancePrecomputerShader.hpp
    src/rendering/shaders/SkyboxShader.cpp
    src/rendering/shaders/GUIRectangleShader.cpp
    src/rendering/shaders/SkyboxShader.hpp
    src/rendering/shaders/ShaderHolder.hpp
    src/rendering/shaders/PBRShaderManager.cpp
    src/rendering/shaders/PBRShader.cpp
    src/rendering/shaders/GUIRectangleShader.hpp
    src/rendering/shaders/IrradiancePrecomputerShader.cpp
    src/rendering/shaders/SpecularPrefilterShader.cpp
    src/rendering/shaders/EquirectangularMapperShader.cpp
    src/rendering/shaders/BRDFIntegrationMapperShader.cpp
    src/rendering/shaders/ColoredTextShader.cpp
    src/rendering/shaders/PBRShader.hpp
    src/rendering/shaders/PBRShaderManager.hpp
    src/rendering/shaders/ColoredTextShader.hpp
    src/rendering/shaders/SpecularPrefilterShader.hpp
    src/rendering/shaders/BRDFIntegrationMapperShader.hpp
    src/rendering/VkFormatInfo.hpp
    src/rendering/FPSMeter.cpp
    src/rendering/GLTFLoading.cpp
    src/rendering/RenderingServer.cpp
    src/rendering/GLTFToNode.cpp
    src/rendering/Texture.cpp
    src/rendering/TextureFromKTX.cpp
    src/rendering/TextureFromRGBE.cpp
    src/uninitialized_error.hpp
    src/physics/BulletPhysicsServer.cpp
    src/utils/json_conversion.hpp
    src/utils/shader_loader.hpp
    src/utils/Channel.hpp
    src/utils/shader_loader.cpp
    src/utils/glm_bullet_conversion.hpp
    src/utils/math.hpp
    src/structs/shapes/SphereShape.cpp
    src/structs/shapes/CylinderShape.cpp
    src/structs/shapes/BoxShape.cpp
    src/structs/shapes/Shape.cpp
    src/shaders/equirectangular_mapper.frag
    src/shaders/skybox.frag
    src/shaders/pbr_shader.vert
    src/shaders/colored_text.vert
    src/shaders/skybox.vert
    src/shaders/gui_rectangle.vert
    src/shaders/pbr_shader.frag
    src/shaders/colored_text.frag
    src/shaders/gui_rectangle.frag
    src/shaders/brdf_integration_mapper.frag
    src/shaders/specular_prefilter.vert
    src/shaders/brdf_integration_mapper.vert
    src/shaders/irradiance_precomputer.frag
    src/shaders/specular_prefilter.frag
    src/shaders/irradiance_precomputer.vert
    src/shaders/equirectangular_mapper.vert
    src/gui/FreeTypeFont.cpp
    src/SceneFile.cpp
    src/SceneJSON.cpp
    src/node_registration.cpp
    src/nodes/rendering/SpectatorCameraNode.cpp
    src/nodes/rendering/PBRDrawableNode.cpp
    src/nodes/rendering/DrawableNode.cpp
    src/nodes/rendering/PointLightNode.cpp
    src/nodes/rendering/CameraNode.cpp
    src/nodes/rendering/SpotLight.cpp
    src/nodes/physics/BulletRigidBodyNode.cpp
    src/nodes/SpatialNode.cpp
    src/nodes/gui/GUINode.cpp
    src/nodes/gui/CompleteGUINode.cpp
    src/nodes/gui/ButtonNode.cpp
    src/nodes/gui/TextNode.cpp
    src/nodes/CompleteSpatialNode.cpp
    src/logger.cpp
    src/GameInstance.cpp
)

set(PUBLIC_HEADERS
    include/LLEngine/rendering/RenderingServer.hpp
    include/LLEngine/rendering/Skybox.hpp
    include/LLEngine/rendering/Texture.hpp
    include/LLEngine/rendering/Mesh.hpp
    include/LLEngine/rendering/GLFWWindow.hpp
    include/LLEngine/rendering/Material.hpp
    include/LLEngine/rendering/Window.hpp
    include/LLEngine/Transform.hpp
    include/LLEngine/physics/shapes/BoxShape.hpp
    include/LLEngine/physics/shapes/SphereShape.hpp
    include/LLEngine/physics/shapes/Shape.hpp
    include/LLEngine/physics/shapes/CylinderShape.hpp
    include/LLEngine/physics/BulletPhysicsServer.hpp
    include/LLEngine/SceneJSON.hpp
    include/LLEngine/node_registration.hpp
    include/LLEngine/Event.hpp
    include/LLEngine/SceneFile.hpp
    include/LLEngine/gui/FreeTypeFont.hpp
    include/LLEngine/gui/GUITexture.hpp
    include/LLEngine/gui/GUITransform.hpp
    include/LLEngine/GLTF.hpp
    include/LLEngine/logger.hpp
    include/LLEngine/Key.hpp
    include/LLEngine/nodes/rendering/PBRDrawableNode.hpp
    include/LLEngine/nodes/rendering/CameraNode.hpp
    include/LLEngine/nodes/rendering/PointLightNode.hpp
    include/LLEngine/nodes/rendering/DrawableNode.hpp
    include/LLEngine/nodes/rendering/SpectatorCameraNode.hpp
    include/LLEngine/nodes/rendering/SpotLight.hpp
    include/LLEngine/nodes/physics/BulletRigidBodyNode.hpp
    include/LLEngine/nodes/SpatialNode.hpp
    include/LLEngine/nodes/CompleteSpatialNode.hpp
    include/LLEngine/nodes/gui/GUINode.hpp
    include/LLEngine/nodes/gui/CompleteGUINode.hpp
    include/LLEngine/nodes/gui/TextNode.hpp
    include/LLEngine/nodes/gui/ButtonNode.hpp
    include/LLEngine/FPSMeter.hpp
    include/LLEngine/GameInstance.hpp
)

find_package(Threads)
add_library(llengine SHARED ${SOURCES})

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

# OpenGL.
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(llengine OpenGL)

# Zstd.
find_package(ZSTD REQUIRED)
target_link_libraries(llengine zstd)

# Bullet.
find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIR})
target_link_libraries(llengine ${BULLET_LIBRARIES})

# nlohmann-json.
find_package(nlohmann_json REQUIRED)
target_link_libraries(llengine nlohmann_json::nlohmann_json)

# glm.
find_package(glm REQUIRED)
target_link_libraries(llengine glm::glm)

# fmtlib.
find_package(fmt REQUIRED)
target_link_libraries(llengine fmt::fmt)

# libktx.
find_package(Ktx REQUIRED)
target_link_libraries(llengine KTX::ktx)

# FreeType.
find_package(Freetype REQUIRED)
target_link_libraries(llengine Freetype::Freetype)

# Copy shaders to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/res")
