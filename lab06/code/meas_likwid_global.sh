#!/bin/sh

OUT=./power_global_likwid_nonv.txt
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
        echo "$(sudo likwid-powermeter ./build/segmentation $filename $cluster $output_img)";
    done
done