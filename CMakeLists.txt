cmake_minimum_required(VERSION 3.0)

# Only define the project if this is the root project
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    project("timer_lib" VERSION 3.0
                 DESCRIPTION "A header-only C++ library that implements an easy-to-use timer class as well as a PreciseTime class using std::chrono."
                 HOMEPAGE_URL "https://github.com/Jakobimatrix/timer")
endif()


add_library(timer_lib INTERFACE)

target_include_directories(timer_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  target_compile_features(timer_lib INTERFACE cxx_std_17)
endif()


option(TEST_SETTINGS "Build tests for timer_lib" TRUE)
if(TEST_SETTINGS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_subdirectory(executable)
endif()



