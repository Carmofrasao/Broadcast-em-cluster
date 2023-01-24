#!/bin/bash
echo "------------- COPIAR (ctrl-c) somente a partir da linha abaixo: -----------"

echo "Executando 10 vezes com [2650000] msgs de [8]B (bloqueante):"    
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 2650000 8 2 | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [21500] msgs de [1000]B bytes (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 21500 1000 2 | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [5500] msgs de [4000]B (bloqueante):"  
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 5500 4000 2 | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [1400] msgs de [16000]B (bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 1400 16000 2 | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [4600000] msgs de [8]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 4600000 8 2 -nbl | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [40000] msgs de [1000]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 40000 1000 2 -nbl | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [10000] msgs de [4000]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 10000 4000 2 -nbl | grep -oP '(?<=Latencia: )[^ ]*'
done
echo "Executando 10 vezes com [2300] msgs de [16000]B (não bloqueante):"
for vez in {1..10}  # 10 vezes
do
    mpirun -np 2 ./PingPongMPI 2300 16000 2 -nbl | grep -oP '(?<=Latencia: )[^ ]*'
done