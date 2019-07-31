#!/bin/bash

EBPF_PATH=../
EBPF_BENCHMARKS=${EBPF_PATH}/ebpf-samples

EBPF_CMD=${EBPF_PATH}/check
#DOMAINS="interval zoneCrab"
DOMAINS="zoneCrab"

TOOL_NAME=crab-epbf

PREFIX=${TOOL_NAME}_$(date +"%m%d%y%H%M")

for dom in $DOMAINS
do
    rm -f log_${dom}.txt
    echo -n "Running ${TOOL_NAME} with $dom ... "
    echo "File,Result,Cpu,Mem"  1>> ${PREFIX}_${dom}.csv
    for f in ${EBPF_BENCHMARKS}/*/*.o
    do
	sections=($(${EBPF_CMD} $f -l))
	for s in "${sections[@]}"
	do
	    echo "${EBPF_CMD} ${f} ${s} --domain=${dom}" >> log_${dom}.txt
	    echo -n $f:$s 1>> ${PREFIX}_${dom}.csv
	    o=$(${EBPF_CMD} ${f} ${s} --domain=${dom} 2>>log_${dom}.txt)
	    echo -n ",$o" 1>> ${PREFIX}_${dom}.csv
	    echo 1>> ${PREFIX}_${dom}.csv
	done
    done
    echo "DONE"
done


