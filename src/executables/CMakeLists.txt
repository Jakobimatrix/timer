add_executable(example_timer src/example_timer.cpp)

install(TARGETS example_timer DESTINATION bin)

target_link_libraries(example_timer 
  PRIVATE
  timer_lib_1.0.0
  BuildSettings_EXE
) 


