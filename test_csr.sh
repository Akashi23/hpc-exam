#!/bin/bash

filename="./results_csr_linux.txt"
[ -f $filename ] && rm $filename
touch $filename

echo "Hardware info:"
SYSTEM_NAME=$(uname)

echo $SYSTEM_NAME

if [[ $SYSTEM_NAME = "Darwin" ]]
then
    system_profiler SPSoftwareDataType SPHardwareDataType
    system_profiler SPSoftwareDataType SPHardwareDataType >> $filename
elif [[ $SYSTEM_NAME = "Linux" ]]
then 
    lshw -short
    lshw -short >> $filename
else
    echo "You're not Linux or Mac user."
    exit 1;
fi

echo
echo

FILES="1138_bus.mtx af23560.mtx airfoil_2d.mtx bcsstk25.mtx c-58.mtx cavity22.mtx"

for file in $FILES
do
    echo "${file} executing..."
    for i in {1..8}
    do
        ./chapter $file $i >> $filename
        sleep 1
    done
done