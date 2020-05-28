
#
# library
#

add_subdirectory(library)

#
# unit tests
#

if (UNIT_TESTS)
  add_subdirectory(tests)
endif()
