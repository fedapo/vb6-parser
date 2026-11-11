find_package(Catch2 REQUIRED)
find_package(doctest REQUIRED)
find_package(GTest REQUIRED)
find_package(ut REQUIRED)

add_executable(vb6_parser.ut
    vb6_parser.ut.cpp
    vb6_parser_expressions.ut.cpp
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
    vb6_parser_expressions.doctest.cpp
    vb6_parser_statements.doctest.cpp
)

target_link_libraries(vb6_parser.doctest
PRIVATE
    vb6_parser_lib
    Boost::boost
    doctest::doctest
)

# --------------------------------

add_executable(vb6_parser.catch
    vb6_parser.catch.cpp
    vb6_parser_expressions.catch.cpp
    vb6_parser_statements.catch.cpp
)

target_link_libraries(vb6_parser.catch
PRIVATE
    vb6_parser_lib
    Boost::boost
    Catch2::Catch2
)

# --------------------------------

add_executable(vb6_parser.gtest
    vb6_parser.gtest.cpp
    vb6_parser_expressions.gtest.cpp
    vb6_parser_statements.gtest.cpp
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

include(Catch)
catch_discover_tests(vb6_parser.catch TEST_PREFIX catch2: PROPERTIES LABELS "catch2" LABELS "discover_tests")
