add_executable(llengine_demo
    src/main.cpp
    src/FPSTextNode.cpp
    src/MainCamera.cpp
    src/BulletNode.cpp
)

target_link_libraries(llengine_demo llengine)

# Copy resources.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})