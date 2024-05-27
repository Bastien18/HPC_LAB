#!/bin/sh

#OUT=./time_measure_init.txt
#OUT=./time_measure_kmeanpp_vect.txt
OUT=./time_measure_vect_malloc_opti.txt
INPUT=../img/*[.png]
INPUT_DIR=../img/
OUTPUT_DIR=../img/output/

rm $OUT;
rm -r $OUTPUT_DIR;
mkdir $OUTPUT_DIR;

for filename in $INPUT;
do
    echo $filename >> $OUT;
    for cluster in 1 2 5 10 20 50;
    do
        output_img=$(echo $filename | sed -e "s|$INPUT_DIR|$OUTPUT_DIR|g" | sed -e "s|.png|-seg$cluster.png|g");
        echo "Nb cluster = $cluster time is $(./build/segmentation $filename $cluster $output_img)" >> $OUT;
    done
done