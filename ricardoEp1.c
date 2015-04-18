#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t mutex;
pthread_barrier_t barrier;

int d;
int n;
char* tipoVeloc;
int** pista;

/***** prototipos das funcoes *****/
int parserEntrada(int, char**);				//OK
void criaPista();							//OK
void geraLargada();							//OK
int retornaPosicao(int);					//OK
int verificaSePossicaoEstaLivre(int);		//OK
int velocidade(int);						//OK
void imprimePista();						//OK

/**********************************/

void *ciclista(void *a){  
	long idCiclista = (long) a; 
	int pos, numVolta;

	pos = retornaPosicao(idCiclista);
	// printf("Thread: %ld 	Posicao: %d 	Rodada: 0\n", idCiclista, pos);

  	for(numVolta = 1; numVolta < 5; numVolta++){

		sem_wait(&mutex);

	    pos = velocidade(pos);
	    
		sem_post(&mutex);

		printf("Thread: %ld 	Posicao: %d 	Rodada: %d\n", idCiclista, pos, numVolta);
		
		//barreira
	    pthread_barrier_wait(&barrier);
		
	}

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
	imprimePista();

    /****************************************/
	pthread_barrier_init(&barrier, NULL, n);

	if(sem_init(&mutex,0,1)){
        printf("Erro ao criar o semáforo :(\n");
        return(2);
    }

    for(i = 0; i < n; i++){
        if(pthread_create(&corredores[i], NULL, ciclista, (void *) i)){
            printf("\n ERROR creating thread %ld", i);
            exit(1);
        }
    }

    for(i = 0; i < n; i++){
        if(pthread_join(corredores[i], NULL)){
            printf("\n ERROR joining thread");
            exit(1);
        }
    }

    pthread_exit(NULL);

	return 0;
}

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
/******************* FUNCOES AUXILIARES ******************************/

int retornaPosicao(int id){
  int i;

  for(i = 0; i < n; i++){
    if(pista[i][0] == id) break;
  }

  return i;
}

int verificaSePossicaoEstaLivre(int pos){
  int j;

  for(j = 0; j < 4; j++){
    if(pista[pos][j] == -1) return 1;
  }

  return 0;   
}

int velocidade(int pos){
    sleep(72/1000);

    if((pos+1 != d) && verificaSePossicaoEstaLivre(pos+1) != -1) 
      return pos+1;

    else if((pos+1 == d) && verificaSePossicaoEstaLivre(0) != -1)
      return 0;

    else 
      return pos;
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