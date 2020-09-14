# timer
A simple timer class easy to use. useing std::chrono

### Features so far PreciseTime class:
 * adding/substracting with considering the unit: 1[s] + 1[s] -> ok; 1[s] + 1[s^2] -> error
 * division/multiplication with
 * a) int/double/etc
 * b) PreciseTime class while keeping track of the unit: 1[s] * 1[s] = 1[s^2]
 * calculateing the square root of a PreciseTime keeping track of the unit: sqrt(4[s^2]) = 2[s].
 * Comparisons: {==, !=, <=, >=, <, >}
 * Nice print output: std::cout << my_precise_time << std::endl;
 * Max Time: h: [9223372036854775807] m: [59]  s: [59]  ms: [999] us: [999] ns: [999]
 * Min Time: h: [-9223372036854775808] m: [-59]  s: [-59]  ms: [-999] us: [-999] ns:[-999]

### Features so far Timer class:
 * Timer: Useing the PreciseTime class to track time.
 * No need to register new timers.
 * As many timers as you like.
 * On demand output of max, min, mean, standard deviation for all timers.
 
## Todos
 - [x] Better conversations between ns, ms ,s
 - [x] add commentary
 - [x] add an example usage
 - [ ] have a nice way to print measurements to file
 - [ ] compute Frequency Distribution Table
 - [ ] Tests
 
## Build the project (Linux)
* If you use CMake to build your project place this repro inside 'path' and use **add_subdirectory(path/
timer)** and **target_link_libraries(your_lib timer_lib)**
* Or just include the header
