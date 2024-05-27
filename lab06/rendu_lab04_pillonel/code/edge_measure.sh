#!/bin/sh

OUT=./time_measure_edge_simd_rgb2gs.txt
INPUT=../img/*[.png]
INPUT_DIR=../img/
OUTPUT_DIR=../img/output_edge/

rm $OUT;
rm -r $OUTPUT_DIR;
mkdir $OUTPUT_DIR;

for filename in $INPUT;
do
    echo $filename >> $OUT;
    for method in 1 2;
    do
        output_img=$(echo $filename | sed -e "s|$INPUT_DIR|$OUTPUT_DIR|g" | sed -e "s|.png|-edge$method.png|g");
        { time ./build/edge_detection $filename $output_img $method; } 2>&1 | tee -a "$OUT";
    done
done