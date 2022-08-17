add_subdirectory(lib)
if(CLANG_INCLUDE_TESTS)
  add_subdirectory(test)
  add_subdirectory(unittests)
endif()

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
