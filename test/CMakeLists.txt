find_package(Boost CONFIG REQUIRED COMPONENTS system)
find_package(doctest CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(ut CONFIG REQUIRED)

add_executable(vb6_parser.ut
    vb6_parser.ut.cpp
)

target_link_libraries(vb6_parser.ut
PRIVATE
    vb6_parser_lib
    Boost::boost
    Boost::ut
)

# --------------------------------

add_executable(vb6_parser.doctest
    vb6_parser.doctest.cpp
)

target_link_libraries(vb6_parser.doctest
PRIVATE
    vb6_parser_lib
    Boost::boost
    doctest::doctest
)

# --------------------------------

add_executable(vb6_parser.gtest
    vb6_parser_statements.gtest.cpp
    vb6_parser.gtest.cpp
    vb6_parser_test_main.cpp
)

target_link_libraries(vb6_parser.gtest
PRIVATE
    vb6_parser_lib
    Boost::boost
    GTest::gtest
)

# --------------------------------

add_test(NAME vb6_parser.ut COMMAND vb6_parser.ut)
set_tests_properties(vb6_parser.ut PROPERTIES LABELS "ut;add_test")

include(GoogleTest)
gtest_discover_tests(vb6_parser.gtest TEST_PREFIX gtest: PROPERTIES LABELS "gtest" LABELS "discover_tests")

#add_test(NAME vb6_parser.doctest COMMAND vb6_parser.doctest)
include(doctest)
doctest_discover_tests(vb6_parser.doctest TEST_PREFIX doctest: PROPERTIES LABELS "doctest" LABELS "discover_tests")
