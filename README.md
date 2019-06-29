# timer
A simple timer class easy to use. useing std::chrono

### Features so far:
 * Calculate the duration of a function call multiple times even when multithreading.
 * Get min, max, average, variance of the measurement.
 
## Todos
 - [ ] Better conversations between ns, ms ,s
 - [ ] add commentary
 - [ ] add an example usage
 
## Build the project (Linux)
* If you use CMake to build your project place this repro inside 'path' and use **add_subdirectory(path/
timer)** and **target_link_libraries(your_lib timer_lib)**
* Or just include the header
