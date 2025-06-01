
if (NOT FUZZER_ENABLED)

  # only enable other executables, if Fuzzer is not linked. Fuzzer brings its own main!
  Include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0 # or a later release
  )

  FetchContent_MakeAvailable(Catch2)

  enable_testing()

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

  include(CTest)
  include(Catch)

  catch_discover_tests(test_precise_time)
  catch_discover_tests(test_timer)


endif()
