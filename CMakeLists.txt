cmake_minimum_required(VERSION 3.10)

project(vb6_parser)

set(CMAKE_CXX_STANDARD 20)

#------------------------------------------------------------------------------
# see https://google.github.io/googletest/quickstart-cmake.html
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/7df7853ea02371f6d24ccf4a0cf9e16553d23d05.zip
)
# for Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
#------------------------------------------------------------------------------

add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang>:-Wall> $<$<CXX_COMPILER_ID:GNU,Clang>:-Wextra>)

#find_package(GTest CONFIG REQUIRED) # GoogleTest with vcpkg
find_package(ut CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED QUIET COMPONENTS system)

add_library(vb6_parser_lib
  src/raw_ast_printer.cpp
  src/raw_ast_printer.hpp
  src/color_console.cpp
  src/color_console.hpp
  src/cpp_ast_printer.cpp
  src/cpp_ast_printer.hpp
  src/vb6_ast.hpp
  src/vb6_ast_adapt.hpp
  src/vb6_config.hpp
  src/vb6_error_handler.hpp
  src/vb6_parser.cpp
  src/vb6_parser.hpp
  src/vb6_parser_def.hpp
  src/vb6_parser_functions.cpp
  src/vb6_parser_helper.cpp
  src/vb6_parser_keywords.hpp
  src/vb6_parser_operators.hpp
  src/vb6_parser_statements.cpp
  src/vb6_parser_statements_def.hpp
  src/vb6_ast_printer.cpp
  src/vb6_ast_printer.hpp
  src/visual_basic_x3.hpp
)

target_compile_definitions(vb6_parser_lib
PRIVATE
  -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
  -DBOOST_MPL_LIMIT_LIST_SIZE=30
)

target_link_libraries(vb6_parser_lib
PRIVATE
  Boost::system
)

add_executable(vb6_parser
  src/vb6_parser_main.cpp
  src/vb6_test1.cpp
  src/vb6_test2.cpp
)

target_compile_definitions(vb6_parser
PRIVATE
  -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
  -DBOOST_MPL_LIMIT_LIST_SIZE=30
)

target_link_libraries(vb6_parser
PRIVATE
  vb6_parser_lib
  Boost::system
  Threads::Threads
)

# ---- test

add_executable(vb6_parser.ut
  #src/test_gosub.cpp
  #src/test_grammar_helper_gtest.hpp
  #src/vb6_parser_statements_test.cpp
  src/vb6_parser.ut.cpp
)

target_compile_definitions(vb6_parser.ut
PRIVATE
  -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
  -DBOOST_MPL_LIMIT_LIST_SIZE=30
)

target_link_libraries(vb6_parser.ut
PRIVATE
  vb6_parser_lib
  Boost::ut
  Boost::system
  Threads::Threads
)

add_executable(vb6_parser_test
  src/test_gosub.cpp
  src/test_grammar_helper_gtest.hpp
  src/vb6_parser_statements_test.cpp
  src/vb6_parser_test.cpp
  src/vb6_parser_test_main.cpp
)

target_compile_definitions(vb6_parser_test
PRIVATE
  -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
  -DBOOST_MPL_LIMIT_LIST_SIZE=30
)

target_link_libraries(vb6_parser_test
PRIVATE
  vb6_parser_lib
  gtest_main
  Boost::system
  Threads::Threads
)

#[[
# GoogleTest with vcpkg, probably not a good idea
target_link_libraries(vb6_parser_test
PRIVATE
  vb6_parser_lib
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  Boost::system
  Threads::Threads
)
#]]

set_target_properties(vb6_parser.ut PROPERTIES EXCLUDE_FROM_ALL 1)

include(CTest)
#enable_testing()

include(GoogleTest)
gtest_discover_tests(vb6_parser_test)

add_test(AllTestsInMain vb6_parser_test)
