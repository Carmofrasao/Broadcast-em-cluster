#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chrono.c"

long nmsg;       // o número total de mensagens
long tmsg;       // o tamanho de cada mensagem
int nproc;      // o número de processos MPI
int raiz;       // maquina que ira enviar as mensagens
int processId; 	// rank dos processos
int ni;			// tamanho do vetor contendo as mensagens

chronometer_t pingPongTime;

#define DEBUG 0

// mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 2 32 8

void verificaVetores( long ping[], long pong[], int ni )
{
   static int twice = 0;
   int ping_ok = 1;
   int pong_ok = 1;
   int i, rank;
      
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );   
   
   if( twice == 0 ) {
  
      if (rank == 0) {
      
          for( i=0; i<ni; i++ ) {
            if( ping[i] != i+1 ) { ping_ok = 0; break; }
            if( pong[i] != 0   ) { pong_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank 0, initial value of ping[%d] = %ld (wrong!)\n", i, ping[i] );
          if( !pong_ok )
             fprintf(stderr, 
               "--------- rank 0, initial value of pong[%d] = %ld (wrong!)\n", i, pong[i] );
          if( ping_ok && pong_ok )
             fprintf(stderr, 
               "--------- rank 0, initial value of ping and pong are OK\n" );

      } else if (rank == 1) {
      
          for( i=0; i<ni; i++ ) {
            if( ping[i] != 0      ) { ping_ok = 0; break; }
            if( pong[i] != i+ni+1 ) { pong_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank 1, initial value of ping[%d] = %ld (wrong!)\n", i, ping[i] );
          if( !pong_ok )
             fprintf(stderr, 
               "--------- rank 1, initial value of pong[%d] = %ld (wrong!)\n", i, pong[i] );
          if( ping_ok && pong_ok )
             fprintf(stderr, 
               "--------- rank 1, initial values of ping and pong are OK\n" );
      }          
   }   // end twice == 0
   
   if( twice == 1 ) {
  
          for( i=0; i<ni; i++ ) {
            if( ping[i] != i+1      ) { ping_ok = 0; break; }
            if( pong[i] != i+ni+1   ) { pong_ok = 0; break; }
          }
          if( !ping_ok )
             fprintf(stderr, 
               "--------- rank %d, FINAL value of ping[%d] = %ld (wrong!)\n", rank, i, ping[i] );
          if( !pong_ok )
             fprintf(stderr, 
               "--------- rank %d, FINAL value of pong[%d] = %ld (wrong!)\n", rank, i, pong[i] );
          if( ping_ok && pong_ok )
             fprintf(stderr, 
               "--------- rank %d, FINAL values of ping and pong are OK\n", rank );

   }  // end twice == 1
   
   ++twice;
   if( twice > 2 )
      fprintf(stderr, 
               "--------- rank %d, verificaVetores CALLED more than 2 times!!!\n", rank );     
}        

// int My_Bcast(long int *inmsg, int count, MPI_LONG, raiz, MPI_COMM_WORLD){

// 	int rc;
// 	long int *outmsg = (long int*)calloc(ni, sizeof(long int));

// 	rc = MPI_Send(&inmsg[i], 1, MPI_LONG, dest, tag, MPI_COMM_WORLD);
// 	rc = MPI_Recv(&outmsg[i], 1, MPI_LONG, source, tag, MPI_COMM_WORLD, &Stat);

// }

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
        if(argc == 5){
            if (strcmp(argv[3], "-r") == 0)
                raiz = atoi(argv[4]);
        }
	}

	ni = tmsg/8;
	MPI_Status Stat;

	long int *inmsg = (long int*)calloc(ni, sizeof(long int));

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);

	if(processId == 0){
		for(long int i = 1; i <= ni; i++)
			inmsg[i-1] = i;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(processId == 0){
		chrono_reset(&pingPongTime);
		chrono_start(&pingPongTime);
	}

	int count = 1;
	for(int m = 0; m < nmsg; m++)
		for(int i = 0; i < ni; i++){
			MPI_Bcast(&inmsg[i], count, MPI_LONG, raiz, MPI_COMM_WORLD);
		}

	// int dest, source, tag = 1;
	// for(int i = 0; i < 8; i++){
	// 	if ( processId == raiz ) {
	// 		dest = 1;
	// 		source = 1;
	// 		for(int m = 0; m < nmsg; m++)
	// 			for(int i = 0; i < ni; i++){
	// 				My_Bcast(&inmsg[i], count, MPI_LONG, raiz, MPI_COMM_WORLD)
	// 			}
	// 	}
	// 	raiz = (raiz+i)%nproc;
	// }

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

	free(inmsg);

	MPI_Finalize( );
	return 0;
}