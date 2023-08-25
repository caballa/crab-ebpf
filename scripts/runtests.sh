#!/bin/bash

EBPF_PATH=../
EBPF_BENCHMARKS=${EBPF_PATH}/ebpf-samples

EBPF_CMD=${EBPF_PATH}/check
#DOMAINS="interval zoneCrab"
DOMAINS="zoneCrab"

TOOL_NAME=crab-epbf

PREFIX=${TOOL_NAME}_$(date +"%m%d%y%H%M")
CSV_FILE=${PREFIX}_${dom}.csv

for dom in $DOMAINS
do
    rm -f log_${dom}.txt
    echo -n "Running ${TOOL_NAME} with $dom ... "
    echo "File,Result,Cpu,Mem"  1>> ${CSV_FILE}
    for f in ${EBPF_BENCHMARKS}/*/*.o
    do
	sections=($(${EBPF_CMD} $f -l))
	for s in "${sections[@]}"
	do
	    echo "${EBPF_CMD} ${f} ${s} --domain=${dom}" >> log_${dom}.txt
	    echo -n $f:$s 1>> ${CSV_FILE}
	    o=$(${EBPF_CMD} ${f} ${s} --domain=${dom} 2>>log_${dom}.txt)
	    echo -n ",$o" 1>> ${CSV_FILE}
	    echo 1>> ${CSV_FILE}
	done
    done
    echo "DONE"
done


num_false=$(grep -c "FALSE" ${CSV_FILE} 2>&1)
num_true=$(grep -c "TRUE" ${CSV_FILE} 2>&1)

echo "FALSE=$num_false"
echo "TRUE=$num_true"

if [ ${num_false} -ne 3 ]; then
    echo "Number of fail tests should be 3 but found ${num_false}"
    exit 1
fi

if [ ${num_true} -ne 257 ]; then
    echo "Number of true tests should be 257 but found ${num_true}"
    exit 1
fi

exit 0
