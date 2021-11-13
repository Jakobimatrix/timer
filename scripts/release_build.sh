cd ../build
cmake  .. -DTEST_SETTINGS=True -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest --output-on-failure
