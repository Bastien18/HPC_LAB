#!/bin/sh

OUT=./power_marker_perf_nonv.txt
INPUT=../img/*[.png]
INPUT_DIR=../img/
OUTPUT_DIR=../img/output/

rm $OUT;
rm -r $OUTPUT_DIR;
mkdir $OUTPUT_DIR;

mkfifo perf_fifo.ctl
mkfifo perf_fifo.ack

exec {perf_ctl_fd}<>perf_fifo.ctl
exec {perf_ack_fd}<>perf_fifo.ack

export PERF_CTL_FD=${perf_ctl_fd}
export PERF_ACK_FD=${perf_ack_fd}

for filename in $INPUT;
do
    echo $filename >> $OUT;
    for cluster in 2 10 40;
    do
        output_img=$(echo $filename | sed -e "s|$INPUT_DIR|$OUTPUT_DIR|g" | sed -e "s|.png|-seg$cluster.png|g");
        echo "$(sudo perf stat -e power/energy-pkg/ --delay=-1 --control fd:${perf_ctl_fd},${perf_ack_fd} -- ./build/segmentation_perf $filename $cluster $output_img)";
    done
done