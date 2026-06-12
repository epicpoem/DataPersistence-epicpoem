cmake_minimum_required(VERSION 3.16)
project(DataPersistence CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(sample_lib
    src/JsonSampleRepository.cpp
)
target_include_directories(sample_lib PUBLIC include)
target_link_libraries(sample_lib PUBLIC nlohmann_json::nlohmann_json)
if(MSVC)
    target_compile_definitions(sample_lib PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(sample_lib PUBLIC /utf-8)
endif()

add_executable(DataPersistence
    src/main.cpp
)
target_link_libraries(DataPersistence PRIVATE sample_lib)

enable_testing()
add_executable(DataPersistence_test
    test/SampleRepositoryTest.cpp
)
target_link_libraries(DataPersistence_test PRIVATE
    sample_lib
    GTest::gtest_main
    GTest::gmock
)
include(GoogleTest)
gtest_discover_tests(DataPersistence_test)
