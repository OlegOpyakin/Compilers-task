#!/bin/bash

for input_file in tests/*.txt; do
    base_name=$(basename "$input_file" .txt)

    { time ./a.out < "$input_file" > output.txt; } 2> time.txt

    diff output.txt answers/"$base_name".txt > /dev/null

    if [ $? -eq 0 ]; then
        echo "$base_name: Success"
    else
        echo "$base_name: Failure"
    fi

    cat time.txt
done

rm output.txt time.txt

