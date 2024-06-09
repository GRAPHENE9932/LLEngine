set(SOURCES
    src/main.cpp
    src/FPSTextNode.cpp
    src/MainCamera.cpp
    src/BulletNode.cpp
    src/ExposureTextNode.cpp
    src/ResolutionTextNode.cpp
    src/FadingPointLightNode.cpp
)

# Disable the command line window on Windows.
if (WIN32)
    add_executable(llengine_demo WIN32 ${SOURCES})
    target_link_options(llengine_demo PRIVATE "/SUBSYSTEM:WINDOWS;/ENTRY:mainCRTStartup")
else()
    add_executable(llengine_demo ${SOURCES})
endif()

target_link_libraries(llengine_demo llengine)

# Copy resources.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})