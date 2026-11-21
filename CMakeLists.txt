cmake_minimum_required(VERSION 3.20)
project(vr_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VRE_BUILD_TESTS "Build the test suite" ON)
option(VRE_ENABLE_STEAMVR "Compile the SteamVR backend (stub until SDK integrated)" ON)

add_library(vre
    src/core/backend_select.cpp
    src/core/null_backend.cpp
)
target_include_directories(vre PUBLIC include)
target_compile_features(vre PUBLIC cxx_std_17)

if(VRE_ENABLE_STEAMVR)
    target_sources(vre PRIVATE src/core/steamvr_backend.cpp)
    target_compile_definitions(vre PUBLIC VRE_HAS_STEAMVR=1)
else()
    target_compile_definitions(vre PUBLIC VRE_HAS_STEAMVR=0)
endif()

if(MSVC)
    target_compile_options(vre PRIVATE /W4 /WX)
else()
    target_compile_options(vre PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(VRE_BUILD_TESTS)
    enable_testing()
    add_executable(vre_tests tests/test_backend.cpp)
    target_link_libraries(vre_tests PRIVATE vre)
    add_test(NAME backend_tests COMMAND vre_tests)
endif()
