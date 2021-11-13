cd ../build
cmake  .. -DTEST_SETTINGS=True -DCMAKE_BUILD_TYPE=Debug
cmake -j 8 --build .
ctest --output-on-failure

