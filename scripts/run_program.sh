#!/bin/bash
exe="../build/executable/timing_visualizer"
if [ -e $exe ]; then
    exec $exe
else
    echo "Cannot find executable $exe."
fi

