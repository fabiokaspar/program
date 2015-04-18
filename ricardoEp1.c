#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t mutex;
int d;
int n;
char* tipoVeloc;
int** pista;

/***** prototipos das funcoes *****/
int parserEntrada(int, char**);
void criaPista();
void geraLargada();
void avanca();
void imprimePista();
/**********************************/

void *ciclista(void *a){  
	long idCiclista = (long) a; 

	sem_wait(&mutex);

	//secao critica
	// printf("Sou a Thread: %ld\n", idCiclista);

	sem_post(&mutex);

	return NULL;
}

int main(int argc, char** argv)
{
	parserEntrada(argc, argv);

    long i;
    pthread_t corredores[n];

    /****************************************/
	
	criaPista();    
	geraLargada();
	avanca();
	// imprimePista();

    /****************************************/

	// if(sem_init(&mutex,0,1)){
 //        printf("Erro ao criar o semáforo :(\n");
 //        return(2);
 //    }

 //    for(i = 0; i < n; i++){
 //        if(pthread_create(&corredores[i], NULL, ciclista, (void *) i)){
 //            printf("\n ERROR creating thread %ld", i);
 //            exit(1);
 //        }
 //    }

 //    for(i = 0; i < n; i++){
 //        if(pthread_join(corredores[i], NULL)){
 //            printf("\n ERROR joining thread");
 //            exit(1);
 //        }
 //    }

 //    pthread_exit(NULL);

	return 0;
}

/******************* FUNCOES AUXILIARES ******************************/

int parserEntrada(int argc, char** argv){
	if(argc != 4){
		printf("Erro na quantidade de parametros!\n");
		printf("Entrada: ./ep1 d n u\n");

		return 1;
	}
	else{
		d = atoi(argv[1]);
		n = atoi(argv[2]);
		tipoVeloc = argv[3];

		printf("%d %d %s\n\n", d,n, tipoVeloc);

		return 1;
	}
}

void criaPista(){
	pista = malloc(d*sizeof(int *));
	int i, j;

	for(i = 0; i < d; i++){
		pista[i] = malloc(4*sizeof(int));
	}

	//inicializa a pista com ids -1
	for(i = 0; i < d; i++){
		for(j = 0; j < 4; j++){
			pista[i][j] = -1;
		}
	}
}

void geraLargada(){
	int i, num1, num2, aux;

	srand(time(NULL));

	for(i = 0; i < n; i++)
		pista[i][0] = i;

	for(i = 0; i < 2*n; i++){
		num1 = (rand()%n);
		num2 = (rand()%n);

		aux = pista[num1][0];
		pista[num1][0] = pista[num2][0];
		pista[num2][0] = aux;
	}
}

void avanca(){
	int i;

	imprimePista();

	for(i = n-1; i >= 0; i--){
		printf("\n--------------------\n");
		sleep(1);
		pista[i+1][0] = pista[i][0];
		pista[i][0] = -1;
		imprimePista();
	}
}


/*********** FUNCOES TESTES ********/
void imprimePista(){
	int i, j;
	

	for(i = 0; i < d; i++){
		for(j = 0; j < 4; j++){
			printf("%d   ", pista[i][j]);
		}
		printf("\n");
	}
}

