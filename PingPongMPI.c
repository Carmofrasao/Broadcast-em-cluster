#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chrono.c"
#include <assert.h>

#define USE_MPI_Bcast 1  // do NOT change
#define USE_my_Bcast 2   // do NOT change
//choose either BCAST_TYPE in the defines bellow
//#define BCAST_TYPE USE_MPI_Bcast
#define BCAST_TYPE USE_my_Bcast

const int SEED = 100;

long nmsg;       // o número total de mensagens
long tmsg;       // o tamanho de cada mensagem
int nproc;      // o número de processos MPI
int raiz;       // maquina que ira enviar as mensagens
int processId; 	// rank dos processos
int ni;			// tamanho do vetor contendo as mensagens

chronometer_t pingPongTime;

#define DEBUG 0

// mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 2 32 8

int outro_log(int n){
	double n_double = n;
	int resultado = 0;

	while(n_double > 1){
		
		resultado++;
		n_double = n_double/2;
	}

	return resultado;

}

int pow2(int n){
	
	if(n == 0)
		return 1;
	else{
	
		int res = 1;

		for(int i = 0; i < n; i++)
			res *= 2;
	
		return res;
	}
}

int desc_orig(){
	
	int distancia;

	if(processId >= raiz)
		distancia = processId - raiz;
	else
		distancia = processId - raiz + nproc;
	
	int fase = outro_log(distancia+1);

	int result = pow2(fase-1);

	int origem = (processId - result)%nproc;

	if(origem < 0)
		origem += nproc;

	return origem;
}

int desc_dest(int fase){

	return (pow2(fase) + processId) % nproc;

}

int desc_fase(){

	int distancia;

	if(processId >= raiz)
		distancia = processId - raiz;
	else
		distancia = processId - raiz + nproc;

	if(distancia == 0)
		return 0;
	else
		return outro_log(distancia+1);

}

void My_Bcast(long int *buf, int count, MPI_Datatype tipo, int root, MPI_Comm Comm){

	int rc;
	int orig, dest, num_fases, fase_ini;

	num_fases = outro_log(nproc);

	fase_ini = desc_fase();

	orig = desc_orig();

	MPI_Status Stat;

	if(processId != raiz){
		rc = MPI_Recv(buf, ni, tipo, orig, 0, Comm, &Stat);
	}

	for(int i = fase_ini; i < num_fases; i++){
		dest = desc_dest(i);
		rc = MPI_Send(buf, ni, tipo, dest, 0, Comm);
	}
}

void verifica_my_Bcast( void *buffer, int count, MPI_Datatype datatype,
                        int root, MPI_Comm comm )
{
    int comm_size;
    int my_rank;
    
    MPI_Comm_size( comm, &comm_size );
    MPI_Comm_rank( comm, &my_rank );
    static long *buff = (long *) calloc( count*comm_size, sizeof(long) );
    
    
    // preenche a faixa do raiz com alguma coisa (apenas no raiz)
    if( my_rank == root )
       for( int i=0; i<count; i++ )
          buff[ i ] = i+SEED;
    
    #if BCAST_TYPE == USE_MPI_Bcast
       MPI_Bcast( buff, count, datatype, root, comm );
    #elif BCAST_TYPE == USE_my_Bcast
       My_Bcast( buff, count, datatype, root, comm );
    #else
       assert( BCAST_TYPE == USE_MPI_Bcast || BCAST_TYPE == USE_my_Bcast );
    #endif   
	   
    
    // cada nodo verifica se sua faixa recebeu adequadamente o conteudo
    int ok=1;
    int i;
    for( i=0; i<count; i++ )
       if( buff[ i ] != i+SEED ) {
          ok = 0;
          break;
       }
    // imprime na tela se OK ou nao
    if( ok )
        fprintf( stderr, "MY BCAST VERIF: node %d received ok\n", my_rank );
    else
        fprintf( stderr, "MY BCAST VERIF: node %d NOT ok! local position: %d contains %ld\n",
                           my_rank, i, buff[i] );

   free(buff);      
}

int main(int argc, char *argv[]){

	raiz = 0;

	if (argc < 4){
		printf("usage: mpirun -np <np> %s <nmsg> <tmsg> <nproc> (-r <r>)\n",
			   argv[0]);
		return 0;
	}
	else{
        nmsg = atoi(argv[1]);
		tmsg = atoi(argv[2]);
		if (tmsg % 8 != 0){
			printf("usage: mpirun -np <np> %s <nmsg> <tmsg> <nproc> (-r <r>)\n",
			   argv[0]);
			printf("<tmsg> deve ser multiplo de 8\n");
			return 0;
		}
        nproc = atoi(argv[3]);
        if(argc == 6){
            if (strcmp(argv[4], "-r") == 0)
                raiz = atoi(argv[5]);
        }
	}

	ni = tmsg/8;
	MPI_Status Stat;

	long int *inmsg = (long int*)calloc(ni, sizeof(long int));

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);

	if(processId == raiz){
		for(long int i = 1; i <= ni; i++)
			inmsg[i-1] = i;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(processId == 0){
		chrono_reset(&pingPongTime);
		chrono_start(&pingPongTime);
	}

	for(int m = 0; m < nmsg; m++)
		// MPI_Bcast(inmsg, ni, MPI_LONG, raiz, MPI_COMM_WORLD);
		My_Bcast(inmsg, ni, MPI_LONG, raiz, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);

	if(processId == 0){
		chrono_stop(&pingPongTime);
		chrono_reportTime(&pingPongTime, "pingPongTime");

		// calcular e imprimir a VAZAO (numero de operacoes/s)
		double total_time_in_seconds = (double)chrono_gettotal(&pingPongTime) /
									((double)1000 * 1000 * 1000);
	    double total_time_in_micro = (double)chrono_gettotal(&pingPongTime) /
									((double)1000);
		printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);
		printf("Latencia: %lf us/nmsg\n", (total_time_in_micro / nmsg)/2);
		double MBPS = ((double)(nmsg*tmsg) / ((double)total_time_in_seconds*1000*1000));
		printf("Throughput: %lf MB/s\n", MBPS*(nproc-1));
	}
	
	#if DEBUG == 1
		int rank;
		MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 

		printf("rank %d: ", rank);
		for(long int i = 0; i < 2; i++)
			printf("%ld ", inmsg[i]);
		for(long int i = ni-2; i < ni; i++)
			printf("%ld ", inmsg[i]);
		printf("\n");
	#endif

	verifica_my_Bcast(inmsg, ni, MPI_LONG, raiz, MPI_COMM_WORLD);

	free(inmsg);

	MPI_Finalize( );
	return 0;
}