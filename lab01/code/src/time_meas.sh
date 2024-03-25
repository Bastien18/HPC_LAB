#!/bin/bash
OUT=./time_result.md

rm $OUT;

exclude_pattern="(-edge|-gauss|-grayscale)"

for filename in ../images/*;
do 
    for i in {1..2}
    do
        if [[ ! $filename =~ $exclude_pattern ]]; then
            echo "${filename%.png}-edge$i.png :" >> $OUT;
            { time ./lab01 "$filename" "${filename%.png}-edge$i.png" $i; } 2>&1 | tee -a "$OUT";
            echo "" >> $OUT;
        fi
    done
done