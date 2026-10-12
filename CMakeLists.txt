cmake_minimum_required(VERSION 3.20)
project(synergy_practice_html LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
set(JSON_BuildTests OFF CACHE INTERNAL "")

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    GIT_SHALLOW TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nlohmann_json googletest)

# Generator library (pure functions, testable without the file system)
add_library(gen_lib STATIC
    src/organization.cpp
)
target_include_directories(gen_lib PUBLIC include)
target_link_libraries(gen_lib PUBLIC nlohmann_json::nlohmann_json)

# Executable
add_executable(html_generator src/main.cpp)
target_link_libraries(html_generator PRIVATE gen_lib)

# Tests
enable_testing()
add_executable(gen_tests
    tests/organization_test.cpp
)
target_link_libraries(gen_tests PRIVATE gen_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(gen_tests)
