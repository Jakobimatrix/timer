add_library(timer_lib INTERFACE)

target_include_directories(timer_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

option(BUILD_TESTS "Build tests for timer_lib" TRUE)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(executable)
endif()


