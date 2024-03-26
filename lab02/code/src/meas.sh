#!/bin/bash

exclude_pattern="(_edge1|_edge2)"

for filename in ../images/*.png;
do
    if [[ ! $filename =~ $exclude_pattern ]]; then
        for i in {1..2}
        echo "$(sudo likwid-perfctr -C E:N:16 -g HPC_LAB2_INSTR -m ./lab01 ../images/$filename ../images/${filename%.png}$i.png $i)" > ./perf_meas/instr_meas_${filename%.png}$i.png;
        echo "$(sudo likwid-perfctr -C E:N:16 -g HPC_LAB2_MEM -m ./lab01 ../images/$filename ../images/${filename%.png}$i.png $i)" > ./perf_meas/mem_meas_${filename%.png}$i.png;
    fi
done