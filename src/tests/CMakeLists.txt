
if (TARGET Catch2::Catch2WithMain)

  add_executable(test_precise_time src/test_precise_time.cpp)
  add_executable(test_timer src/test_timings.cpp)

  target_link_libraries(test_timer 
    PRIVATE
    Catch2::Catch2WithMain
    timer_lib_1.0.0
    ${ENVIRONMENT_SETTINGS}
    )

    target_link_libraries(test_precise_time 
    PRIVATE
    Catch2::Catch2WithMain
    timer_lib_1.0.0
    ${ENVIRONMENT_SETTINGS}
    )

  catch_discover_tests(test_precise_time)
  catch_discover_tests(test_timer)


endif()
