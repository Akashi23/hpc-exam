#!/bin/bash

filename="./results_csr_mac.txt"
[ -f $file ] && rm $file
touch $file

echo "Hardware info:"
SYSTEM_NAME=$(uname)

if [[ $SYSTEM_NAME -eq "Darwin" ]]
then
    system_profiler SPSoftwareDataType SPHardwareDataType
    system_profiler SPSoftwareDataType SPHardwareDataType >> $filename
elif [[ $SYSTEM_NAME -eq "Linux" ]]
then 
    lshw -short
    lshw -short >> $filename
else
    echo "Works on two system Linux and Mac."
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
    done
done