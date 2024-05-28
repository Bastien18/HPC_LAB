#!/bin/sh

OUT=./power_marker_likwid_nonv.txt
INPUT=../img/*[.png]
INPUT_DIR=../img/
OUTPUT_DIR=../img/output/

rm $OUT;
rm -r $OUTPUT_DIR;
mkdir $OUTPUT_DIR;

for filename in $INPUT;
do
    echo $filename >> $OUT;
    for cluster in 2 10 40;
    do
        output_img=$(echo $filename | sed -e "s|$INPUT_DIR|$OUTPUT_DIR|g" | sed -e "s|.png|-seg$cluster.png|g");
        echo "$(sudo likwid-perfctr -C E:N:16 -g CLOCK -m ./build/segmentation_likwid $filename $cluster $output_img)";
    done
done