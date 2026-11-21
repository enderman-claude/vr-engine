# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/claude/work/vr-engine/build-off/_deps/enkits-src")
  file(MAKE_DIRECTORY "/home/claude/work/vr-engine/build-off/_deps/enkits-src")
endif()
file(MAKE_DIRECTORY
  "/home/claude/work/vr-engine/build-off/_deps/enkits-build"
  "/home/claude/work/vr-engine/build-off/_deps/enkits-subbuild/enkits-populate-prefix"
  "/home/claude/work/vr-engine/build-off/_deps/enkits-subbuild/enkits-populate-prefix/tmp"
  "/home/claude/work/vr-engine/build-off/_deps/enkits-subbuild/enkits-populate-prefix/src/enkits-populate-stamp"
  "/home/claude/work/vr-engine/build-off/_deps/enkits-subbuild/enkits-populate-prefix/src"
  "/home/claude/work/vr-engine/build-off/_deps/enkits-subbuild/enkits-populate-prefix/src/enkits-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/claude/work/vr-engine/build-off/_deps/enkits-subbuild/enkits-populate-prefix/src/enkits-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/claude/work/vr-engine/build-off/_deps/enkits-subbuild/enkits-populate-prefix/src/enkits-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
