#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CICLISTA 100
#define NITER 2

pthread_barrier_t barrier[NUM_CICLISTA];

/****** variaveis globais *********/

int** pista;
int d;
int n, n_barr;
int voltaDoLider, idDoLider;
char* tipoVeloc;
//int atualVolta;
sem_t mutex;
int posInic[NUM_CICLISTA];
/**********************************/

int parserEntrada(int argc, char** argv){
	if(argc != 4){
		printf("Erro na quantidade de parametros!\n");
		printf("Entrada: ./ep1 d n [u|v]\n");

        exit(1);
	}
	else{
		d = atoi(argv[1]);
		n_barr = n = atoi(argv[2]);

		if(!(tipoVeloc = malloc(2))){
            printf("\n  ERROR allocation memory!\n");
            exit(1);
		}

		//tipoVeloc = argv[3];
        tipoVeloc[0] = 'u';
        tipoVeloc[1] = '\0';

		if((d % 2 == 0 && n > d/2) || (d % 2 == 1 && n > d/2 + 1)){
            printf("Número máximo de ciclistas deve ser: ⌈d/2⌉\n");
            exit(0);
		}

		printf("%d %d %s\n\n", d,n, tipoVeloc);

		return 1;
	}
}

void criaPista(){
	pista = malloc(d * sizeof(int *));
	int i, j;

	for(i = 0; i < d; i++){
		pista[i] = malloc(4 * sizeof(int));
	}

	//inicializa a pista com ids -1
	for(i = 0; i < d; i++){
		for(j = 0; j < 4; j++){
			pista[i][j] = -1;
		}
	}
}

/***********funcoes testes ********/
void imprimePista(){
	int i, j;

	for(i = 0; i < d; i++){
		for(j = 0; j < 4; j++){
			printf("%d   ", pista[i][j]);
		}
		printf("\n");
	}
}

/**********************************/


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

	for(i = 0; i < n; i++)
        posInic[pista[i][0]] = i;
}


/************ a partir daqui não funciona, precisamos pensar ***********/

void adicioneIdPosicaoAtual(int posAtual, int faixaAtual, int idCiclista){
	//sem_wait(&mutex);
	pista[posAtual][faixaAtual] = idCiclista;
	//sem_post(&mutex);
}

void removaIdPosicaoAntiga(int posAnt, int faixaAnt){
    //sem_wait(&mutex);
	pista[posAnt][faixaAnt] = -1;
	//sem_post(&mutex);
}

// retorna a primeira faixa livre da proxima posição ou -1 c.c
int proximaFaixaLivre(int proxPos){
	int j;

	for(j = 0; j < 4; j++){
        // condição respeita a PNMUV => atomico
		if(pista[proxPos][j] == -1)
            return j;
	}

	return -1;
}

int tentaAvancarMetro(int posAtual, int speedRound){
    int free;

	if(posAtual == d-1){
        free = proximaFaixaLivre(0);
	}

    else free = proximaFaixaLivre(posAtual+1);

    if(free != -1){
        usleep(speedRound);
    }

    return free;
}

int configVelocidadeDaVolta(int volta){
    if(strcmp(tipoVeloc, "u") == 0){  // velocidade 50km/h
        //*speedRound = 72000;
        return 72000;
    }

    else {
        if(volta == 0){  //velocidade 25km/h
            //*speedRound = 144000;
            return 144000;
        }

        else {
            srand(time(NULL));

            if((rand() % 2) == 0){
                //*speedRound = 144000;
                return 144000;
            }

            else{
               // *speedRound = 72000;
               return 72000;
            }
        }
    }

    return 144000;
}

void *ciclista(void *a){
  int proxFaixa;
  int volta;
  long ciclista_id = (long) a;
  int posAtual = posInic[ciclista_id];
  int speedRound = 0;
  int faixa = 0;
  int passo;

  speedRound = configVelocidadeDaVolta(0);

  for(passo = 0, volta = 0; volta < 4; passo++) {
    proxFaixa = tentaAvancarMetro(posAtual, speedRound);

    if(ciclista_id == 0)
        printf("\n");

    /** pra dar tempo da thread terminante atualizar pista e n_barr **/
    usleep(100000);

    pthread_barrier_wait(&barrier[n_barr-1]);

   // printf("Barr: %d\n", n_barr);

    if(proxFaixa != -1){
        posAtual++;

        sem_wait(&mutex);
        removaIdPosicaoAntiga(posAtual-1, faixa);
        adicioneIdPosicaoAtual(posAtual%d, proxFaixa, ciclista_id);
        sem_post(&mutex);

        if(posAtual == d){
            posAtual = 0;
            volta++;
            speedRound = configVelocidadeDaVolta(volta);
        }

        faixa = proxFaixa;
    }

    printf("Thread: %ld; #Rodada: %d;   #Volta: %d;  Pos: %d\n", ciclista_id, passo, volta, posAtual);

    /** preciso:
        - chamar adiciona e remove ids na pista - ok
        - definir a velocidade para aquela volta do ciclista - ok
        - definir quando a thread termina (fim da corrida)
    **/

    //imprimePista();
  }

  printf("> FIM THREAD: %ld\n", ciclista_id);

  sem_wait(&mutex);
  removaIdPosicaoAntiga(0, proxFaixa);
  n_barr--;
  sem_post(&mutex);

  return NULL;
}

int main(int argc, char** argv)
{
    long i;
    parserEntrada(argc, argv);
    pthread_t corredores[n];
	criaPista();
	geraLargada();

	imprimePista();
	printf("\n--------------------------------\n");

	voltaDoLider = 0;

    for(i = 0; i < n_barr; i++){
        if(pthread_barrier_init(&barrier[i], NULL, i+1)){
            printf("\n ERROR initializing barrier %ld\n", i);
            exit(1);
        }
    }

	if(sem_init(&mutex, 0, 1)){
        printf("\n ERROR creating semaphore\n");
        return(2);
    }

    for(i = 0; i < n; i++){
        if(pthread_create(&corredores[i], NULL, ciclista, (void *) i)){
            printf("\n ERROR creating thread %ld\n", i);
            exit(1);
        }
    }

    for(i = 0; i < n; i++){
        if(pthread_join(corredores[i], NULL)){
            printf("\n ERROR joining thread\n");
            exit(1);
        }
    }

   /* if(pthread_barrier_destroy(&barrier)) {
        printf("\n ERROR destroying barrier\n");
        exit(1);
    } */

   // pthread_exit(NULL);

	return 0;
}

/************************************/
