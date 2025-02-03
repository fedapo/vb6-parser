cmake_minimum_required(VERSION 3.28)

project(vb6_parser)

set(CMAKE_CXX_STANDARD 23)

add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang>:-Wall> $<$<CXX_COMPILER_ID:GNU,Clang>:-Wextra>)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_compile_definitions(
    BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
    BOOST_MPL_LIMIT_LIST_SIZE=30
)

add_library(vb6_parser_lib
    src/raw_ast_printer.cpp
    src/color_console.cpp
    src/cpp_ast_printer.cpp
    src/vb6_parser.cpp
    src/vb6_parser_functions.cpp
    src/vb6_parser_helper.cpp
    src/vb6_parser_statements.cpp
    src/vb6_ast_printer.cpp

    src/raw_ast_printer.hpp
    src/color_console.hpp
    src/cpp_ast_printer.hpp
    src/vb6_ast.hpp
    src/vb6_ast_adapt.hpp
    src/vb6_config.hpp
    src/vb6_error_handler.hpp
    src/vb6_parser.hpp
    src/vb6_parser_def.hpp
    src/vb6_parser_keywords.hpp
    src/vb6_parser_operators.hpp
    src/vb6_parser_statements_def.hpp
    src/vb6_ast_printer.hpp
    src/visual_basic_x3.hpp
)

target_include_directories(vb6_parser_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(vb6_parser_lib
PRIVATE
    Boost::system
)

add_executable(vb6_parser
    src/vb6_parser_main.cpp
    src/vb6_test1.cpp
    src/vb6_test2.cpp
)

target_link_libraries(vb6_parser
PRIVATE
    vb6_parser_lib
    Boost::system
    Threads::Threads
)

include(CTest)
#enable_testing()

add_subdirectory(test)
