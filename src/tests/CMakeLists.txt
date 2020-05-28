
add_executable(unit_test unit_tests.cpp)
add_test(NAME unit_test COMMAND unit_test)

set_target_properties(
  unit_test
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
