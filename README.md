# LLEngine - Simple 3D OpenGL Game Engine
Under development, currently is not to be used

![Demo screenshot](screenshots/demo_screenshot_0.png)

Contents:
* [Implemented technical features](#tech-features)
* [Build from source](#build-from-source)
    * [Linux](#linux-build)


## Currently implemented technical features <a name="tech-features"/>
- Rendering with OpenGL.
- Camera control with mouse and keyboard.
- Integration of the Bullet3 physics engine.
- Point lights.
- Skybox.
- glTF model loading.
- KTX texture loading (including cubemaps).
- Radiance RGBE (.hdr) texture loading.
- PBR shading.
- Normal maps, roughness maps, metallic maps, ambient occlusion maps.
- FreeType font loading and text rendering.
- Particles with instancing.
- Bloom postprocessing effect.
- HDR.
- Automatic exposure.
- Frustum culling.
- Nodes system.
- Ability to create user nodes.
- Basic logging functionality.

## Build from source <a name="build-from-source"/>
### Linux <a name="linux-build"/>
```
$ git clone https://github.com/GRAPHENE9932/LLEngine.git
$ cd LLEngine
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=1 -G Ninja ..
$ ninja
```