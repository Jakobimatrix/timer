cmake_minimum_required(VERSION 3.0)

project("test_timer_lib" VERSION 1.0
             DESCRIPTION "Boost Tests for timer.hpp"
             HOMEPAGE_URL "https://github.com/Jakobimatrix/timer")

set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

include_directories(${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

find_package(Boost COMPONENTS system unit_test_framework filesystem)

if( Boost_FOUND )
  add_executable(test_timer test_timings.cpp)

  target_link_libraries(test_timer
                        timer_lib
                        ${Boost_FILESYSTEM_LIBRARY}
                        ${Boost_SYSTEM_LIBRARY}
                        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                        )

  # declares a test with executable
  add_test(NAME timer_test COMMAND test_timer)
else()
  MESSAGE("BOOST NOT FOUND!! CAN NOT RUN TESTS.")
endif()
