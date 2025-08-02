add_executable(parser_client
    parser_client.cpp
    test_gosub.cpp
    vb6_test1.cpp
    vb6_test2.cpp
)

target_link_libraries(parser_client
PRIVATE
    vb6_parser_lib
    Boost::boost
)
