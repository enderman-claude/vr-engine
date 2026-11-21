# CMake generated Testfile for 
# Source directory: /home/claude/work/vr-engine
# Build directory: /home/claude/work/vr-engine/build-off
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test("engine_tests" "/home/claude/work/vr-engine/build-off/vre_tests")
set_tests_properties("engine_tests" PROPERTIES  _BACKTRACE_TRIPLES "/home/claude/work/vr-engine/CMakeLists.txt;75;add_test;/home/claude/work/vr-engine/CMakeLists.txt;0;")
subdirs("_deps/glm-build")
subdirs("_deps/enkits-build")
subdirs("_deps/doctest-build")
