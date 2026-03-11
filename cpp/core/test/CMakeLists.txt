add_executable(guidiff_testing config_test.cpp files_test.cpp tools_test.cpp)
target_link_libraries(
  guidiff_testing
  PUBLIC
    ${GTEST_LIB} core_lib ${BOOST_LIB} ${CROW_LIB}
)

gtest_discover_tests(guidiff_testing)
