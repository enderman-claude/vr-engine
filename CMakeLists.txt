cmake_minimum_required(VERSION 3.20)
project(vr_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VRE_BUILD_TESTS "Build the test suite" ON)
option(VRE_ENABLE_STEAMVR "Compile the SteamVR backend (OpenVR)" ON)

include(FetchContent)

# All third-party code is pinned to exact commits. See THIRD_PARTY_NOTICES.md.

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 8d1fd52e5ab5590e2c81768ace50c72bae28f2ed)  # 1.0.3
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(enkits
    GIT_REPOSITORY https://github.com/dougbinks/enkiTS.git
    GIT_TAG 0289cf6ffce86697e1f46cb4eeca1c946bffec71)  # v1.12
set(ENKITS_BUILD_C_INTERFACE OFF CACHE BOOL "" FORCE)
set(ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ENKITS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ENKITS_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(enkits)

add_library(vre
    src/core/backend_select.cpp
    src/core/null_backend.cpp
    src/core/job_system.cpp
)
target_include_directories(vre PUBLIC include)
target_include_directories(vre PRIVATE ${enkits_SOURCE_DIR}/src)
target_link_libraries(vre PUBLIC glm::glm PRIVATE enkiTS)
target_compile_features(vre PUBLIC cxx_std_17)

if(VRE_ENABLE_STEAMVR)
    FetchContent_Declare(openvr
        GIT_REPOSITORY https://github.com/ValveSoftware/openvr.git
        GIT_TAG 0924064316de3effbcd1acf1e309182a2deb1c05)  # v2.15.6
    set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(USE_LIBCXX OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(openvr)
    target_sources(vre PRIVATE src/core/steamvr_backend.cpp)
    target_link_libraries(vre PRIVATE openvr_api)
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
    FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG 2d0a9359a60c51affe2a9bebb1be1dca47868151)  # v2.5.3
    set(DOCTEST_NO_INSTALL ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(doctest)

    enable_testing()
    add_executable(vre_tests tests/test_engine.cpp)
    target_link_libraries(vre_tests PRIVATE vre doctest::doctest)
    add_test(NAME engine_tests COMMAND vre_tests)
endif()
