# Broadcast em cluster

Trabalho feito para a matéria de programação paralela, no curso de Ciencia da Computação, da Universidade Federal do Paraná.

Autor:
Anderson Frasão

Programa para enviar mensagens em bradcast para um cluster.

Para ultilizar o programa, execute o comando make no terminal (dentro da pasta em que o código esta) e execute um desses comandos:

> ./PingPongMPI

ou

> mpirun -np < np > ./PingPongMPI < nmsg > < tmsg > < nproc > (-r < r >)

sendo: 

* np: numero de processos

* nmsg: numero de mensagens a serem enviadas
  
* tmmsg: tamanho das mensagens a serem enviadas 
  
* nproc: numero de pocessos (sim, é redundante, culpa do professor) 
  
* r: raiz que enviara as mensagens (por padrão é 0) 
