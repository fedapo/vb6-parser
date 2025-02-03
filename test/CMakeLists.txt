cmake_minimum_required(VERSION 3.28)

find_package(Boost REQUIRED COMPONENTS system)
find_package(doctest CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(ut CONFIG REQUIRED)
find_package(Threads REQUIRED)

add_executable(vb6_parser.ut
    vb6_parser.ut.cpp
)

target_link_libraries(vb6_parser.ut
PRIVATE
    vb6_parser_lib
    Boost::system
    Boost::ut
    Threads::Threads
)

# --------------------------------

add_executable(vb6_parser.doctest
    vb6_parser.doctest.cpp
)

target_link_libraries(vb6_parser.doctest
PRIVATE
    vb6_parser_lib
    Boost::system
    doctest::doctest
    Threads::Threads
)

# --------------------------------

add_executable(vb6_parser.gtest
    test_gosub.cpp
    vb6_parser_statements.gtest.cpp
    vb6_parser.gtest.cpp
    vb6_parser_test_main.cpp
)

target_link_libraries(vb6_parser.gtest
PRIVATE
    vb6_parser_lib
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    Boost::system
    Threads::Threads
)

# --------------------------------

add_test(NAME vb6_parser.ut COMMAND vb6_parser.ut)

include(GoogleTest)
gtest_discover_tests(vb6_parser.gtest)

#add_test(NAME vb6_parser.doctest COMMAND vb6_parser.doctest)
include(doctest)
doctest_discover_tests(vb6_parser.doctest)