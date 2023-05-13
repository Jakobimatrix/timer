cd ../build
cmake  .. -DTEST_SETTINGS=True -DCMAKE_BUILD_TYPE=Debug
cmake --build .
ctest --output-on-failure

