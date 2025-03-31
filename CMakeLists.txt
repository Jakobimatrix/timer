add_library(timer_lib INTERFACE)

target_include_directories(timer_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

option(TEST_SETTINGS "Build tests for timer_lib" TRUE)
if(TEST_SETTINGS)
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(executable)
endif()


