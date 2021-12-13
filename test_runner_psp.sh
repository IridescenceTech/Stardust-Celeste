#!/bin/bash

for file in ./build/tests/psp_artifact/*.elf
do
    ../ppsspp/build/PPSSPPHeadless "$file" --timeout=10 -l &>psp_output
    if grep -q -E "I stderr: TEST FAILED!|E BREAK!" psp_output
    then 
        echo "TEST $file FAILED!"
        exit 1
    fi
done