include(GoogleTest)

add_executable(llengine_tst
    main.cpp
    gltf_loading.cpp
    frustum_construction.cpp
    plane_transformation.cpp
)

find_package(GTest)
target_link_libraries(llengine_tst PUBLIC llengine GTest::GTest)

gtest_add_tests(TARGET llengine_tst)
