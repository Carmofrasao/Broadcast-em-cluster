#!/bin/bash
echo "------------- COPIAR (ctrl-c) somente a partir da linha abaixo: -----------"

echo "Executando 10 vezes com [1400000] msgs de [8]B (broadcast):"    
for vez in {1..10}  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt -np 2 ./PingPongMPI 1400000 8 2 2> /dev/null | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [10000] msgs de [1000]B bytes (broadcast):"  
for vez in {1..10}  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 10000 1000 2 2> /dev/null | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [3000] msgs de [4000]B (broadcast):"  
for vez in {1..10}  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 3000 4000 2 2> /dev/null | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [700] msgs de [16000]B (broadcast):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 700 16000 2 2> /dev/null | grep -oP '(?<=Latencia: )[^ ]*'
done
