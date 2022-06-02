include(GoogleTest)

add_executable(${CMAKE_PROJECT_NAME}_tst
    main.cpp
    closest_point.cpp
    rect.cpp
)

find_package(GTest)
target_link_libraries(${CMAKE_PROJECT_NAME}_tst PUBLIC ${CMAKE_PROJECT_NAME}_lib GTest::GTest)

gtest_add_tests(TARGET ${CMAKE_PROJECT_NAME}_tst)
