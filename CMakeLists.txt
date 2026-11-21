cmake_minimum_required(VERSION 3.20)
project(vr_engine VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VRE_BUILD_TESTS "Build the test suite" ON)
option(VRE_ENABLE_STEAMVR "Compile the SteamVR backend (OpenVR)" ON)

# Compile-time eye mode (see NOTES.md, "Flatscreen and eye modes").
set(_vre_eye_modes stereo right left variable no_vr no_render)
set(VRE_EYE_MODE "stereo" CACHE STRING "Compile-time eye mode: ${_vre_eye_modes}")
set_property(CACHE VRE_EYE_MODE PROPERTY STRINGS ${_vre_eye_modes})
if(NOT VRE_EYE_MODE IN_LIST _vre_eye_modes)
    message(FATAL_ERROR "VRE_EYE_MODE must be one of: ${_vre_eye_modes}")
endif()

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

function(vre_apply_warnings target)
    if(MSVC)
        # /external:W0 silences warnings from includes marked external (third-party).
        target_compile_options(${target} PRIVATE /W4 /WX /external:anglebrackets /external:W0)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

add_library(vre
    src/core/backend_select.cpp
    src/core/config.cpp
    src/core/null_backend.cpp
    src/core/job_system.cpp
)
target_include_directories(vre PUBLIC include)
# Third-party headers are SYSTEM includes so their warnings never reach our -Werror/-WX builds.
target_include_directories(vre SYSTEM PUBLIC ${glm_SOURCE_DIR})
target_include_directories(vre SYSTEM PRIVATE ${enkits_SOURCE_DIR}/src)
target_link_libraries(vre PRIVATE enkiTS)
target_compile_features(vre PUBLIC cxx_std_17)
target_compile_definitions(vre PUBLIC VRE_BUILD_EYE_MODE=\"${VRE_EYE_MODE}\")

if(VRE_ENABLE_STEAMVR)
    FetchContent_Declare(openvr
        GIT_REPOSITORY https://github.com/ValveSoftware/openvr.git
        GIT_TAG 0924064316de3effbcd1acf1e309182a2deb1c05)  # v2.15.6
    set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(USE_LIBCXX OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(openvr)
    target_sources(vre PRIVATE src/core/steamvr_backend.cpp)
    target_include_directories(vre SYSTEM PRIVATE ${openvr_SOURCE_DIR}/headers)
    # OpenVR names its static target openvr_api, or openvr_api64 on 64-bit Windows.
    if(TARGET openvr_api64)
        set(VRE_OPENVR_TARGET openvr_api64)
    else()
        set(VRE_OPENVR_TARGET openvr_api)
    endif()
    target_link_libraries(vre PRIVATE ${VRE_OPENVR_TARGET})
    # Static OpenVR: without this, openvr.h declares VR_* as dllimport on Windows.
    target_compile_definitions(vre PRIVATE OPENVR_BUILD_STATIC)
    target_compile_definitions(vre PUBLIC VRE_HAS_STEAMVR=1)
else()
    target_compile_definitions(vre PUBLIC VRE_HAS_STEAMVR=0)
endif()

vre_apply_warnings(vre)

# Studio: project runner and packager (single-file binary or archive folder).
add_library(vre_studio_core STATIC studio/studio.cpp studio/packager.cpp)
target_include_directories(vre_studio_core PUBLIC studio)
target_link_libraries(vre_studio_core PUBLIC vre)
vre_apply_warnings(vre_studio_core)

add_executable(vre_studio studio/main.cpp)
target_link_libraries(vre_studio PRIVATE vre_studio_core)
vre_apply_warnings(vre_studio)

if(VRE_BUILD_TESTS)
    FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG 2d0a9359a60c51affe2a9bebb1be1dca47868151)  # v2.5.3
    set(DOCTEST_NO_INSTALL ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(doctest)

    enable_testing()
    add_executable(vre_tests tests/test_engine.cpp)
    target_link_libraries(vre_tests PRIVATE vre_studio_core doctest::doctest)
    add_test(NAME engine_tests COMMAND vre_tests)
    add_test(NAME studio_smoke COMMAND vre_studio --frames 3)
endif()
