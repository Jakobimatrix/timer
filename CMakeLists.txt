cmake_minimum_required(VERSION 3.0)

project("timer_lib" VERSION 3.0
             DESCRIPTION "A header only c++ library that implements an easy to use timer class as well as an PreciseTime class useing std::chrono."
             HOMEPAGE_URL "https://github.com/Jakobimatrix/timer")


#set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)


option(TEST_SETTINGS "Build tests for ${PROJECT_NAME}" TRUE)
if(TEST_SETTINGS)
  enable_testing()
  add_subdirectory(tests)
endif()

add_subdirectory(executable)



