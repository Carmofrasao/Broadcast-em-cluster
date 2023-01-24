all: PingPongMPI

PingPongMPI: PingPongMPI.c chrono.c
	mpic++ PingPongMPI.c -O3 -g -o PingPongMPI