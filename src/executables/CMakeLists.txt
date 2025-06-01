

if (FUZZER_ENABLED)

else()
  # only enable other executables (like tests), if Fuzzer is not linked. Fuzzer brings its own main!
    
  add_executable(example_timer src/example_timer.cpp)

  # Optionally, you can also install the executable
  install(TARGETS example_timer DESTINATION bin)

  target_link_libraries(example_timer 
    PRIVATE
    timer_lib_1.0.0
    ${ENVIRONMENT_SETTINGS}
  ) 


endif()

