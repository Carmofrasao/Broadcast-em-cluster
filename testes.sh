#!/bin/bash
echo "------------- COPIAR (ctrl-c) somente a partir da linha abaixo: -----------"

echo "Executando 10 vezes com [70000] msgs de [8]B (My broadcast):"    
for vez in $(seq 1 10)  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 70000 8 8 2> /dev/null | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
done
echo "Executando 10 vezes com [10000] msgs de [1000]B bytes (My broadcast):"  
for vez in $(seq 1 10)  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 10000 1000 8 2> /dev/null | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
done
echo "Executando 10 vezes com [3000] msgs de [4000]B (My broadcast):"  
for vez in $(seq 1 10)  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 3000 4000 8 2> /dev/null | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
done
echo "Executando 10 vezes com [700] msgs de [16000]B (My broadcast):"
for vez in $(seq 1 10)  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 700 16000 8 2> /dev/null | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
done
