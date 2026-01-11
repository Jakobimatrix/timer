cmake_minimum_required(VERSION 3.16)

project(precise_timer VERSION 1.0.0 LANGUAGES C CXX)

# defines umbrella targets to link against. It is protected against multiple includes.
add_subdirectory(initRepo) # adds CompilerSetup.cmake and Includes.cmake

manage_library(${PROJECT_NAME} ${PROJECT_VERSION})

if (DEFINED ACCEPTED_TIMER_LIB_VERSION)
  if(NOT "${ACCEPTED_TIMER_LIB_VERSION}" STREQUAL "${PROJECT_VERSION}")
    message(FATAL_ERROR "${PROJECT_NAME} already included with a different version. This Version: ${PROJECT_VERSION}, Other Version: ${ACCEPTED_TIMER_LIB_VERSION}")
  endif()
else()


enable_testing()

setup_catch2_and_ctest()

add_subdirectory(src/timer)
add_subdirectory(src/executables)
add_subdirectory(src/tests)

endif()
