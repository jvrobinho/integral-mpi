#!/bin/bash  

echo "Input number of processes: "
read num_p

for ((i=1;i<=num_p;i++));
    do
        echo "$i process(es)"
        mpirun --hostfile hostfile -np $i integral    
        echo "**********************************************************"
done    