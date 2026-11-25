
cmake_minimum_required(VERSION 3.20)

project(Lab05 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

add_library(lab_core
    src/Patient.cpp
    src/Selection.cpp
    src/Tree.cpp
)

target_include_directories(lab_core PUBLIC include)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lab_core PUBLIC
        -Wall
        -Wextra
        -Wpedantic
    )

    if(ENABLE_ASAN)
        target_compile_options(lab_core PUBLIC
            -fsanitize=address
            -fno-omit-frame-pointer
        )

        target_link_options(lab_core PUBLIC
            -fsanitize=address
        )
    endif()
endif()

add_executable(lab05 src/main.cpp)
target_link_libraries(lab05 PRIVATE lab_core)

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )

    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

add_executable(lab_tests tests/tests.cpp)

target_link_libraries(lab_tests PRIVATE
    lab_core
    GTest::gtest_main
)

add_test(NAME Lab05Tests COMMAND lab_tests)
