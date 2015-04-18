#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CICLISTA 100
#define NITER 2

/****** variaveis globais *********/

int** pista;
int d;
int n;
char* tipoVeloc;
//int atualVolta;
sem_t mutex;
int posInic[NUM_CICLISTA];
/**********************************/

int parserEntrada(int argc, char** argv){
	if(argc != 4){
		printf("Erro na quantidade de parametros!\n");
		printf("Entrada: ./ep1 d n [u|v]\n");

		return 1;
	}
	else{
		d = atoi(argv[1]);
		n = atoi(argv[2]);
		tipoVeloc = argv[3];

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

void geraPosIniCiclista(int ciclista_id){
    int posInicial;

    srand(time(NULL));
    posInicial = (rand() % d);

    while(pista[posInicial][0] != -1) {
        srand(time(NULL));
        posInicial = (rand() % d);
    }

    pista[posInicial][0] = ciclista_id;
    posInic[ciclista_id] = posInicial;
}

void geraLargada(){
	int i;

	for(i = 0; i < n; i++){
		geraPosIniCiclista(i);
	}
}


/************ a partir daqui não funciona, precisamos pensar ***********/

void adicioneIdPosicaoAtual(int posAtual, int faixaAtual, int idCiclista){
	pista[posAtual][faixaAtual] = idCiclista;
}

void removaIdPosicaoAntiga(int posAnt, int faixaAnt){
	pista[posAnt][faixaAnt] = -1;
}

// retorna a primeira faixa livre da proxima posição ou -1 c.c
int proximaFaixaLivre(int proxPos){
	int j;

	for(j = 0; j < 4; j++){
		if(pista[proxPos][j] == -1)
            return j;
	}

	return -1;
}

int tentaAvancarMetro(int posAtual, int speedRound){
    sleep(speedRound/1000);

	if(posAtual == d-1){
        return proximaFaixaLivre(0);
	}

    return proximaFaixaLivre(posAtual+1);
}

void configVelocidadeDaVolta(int volta, int* speedRound){
    if(strcmp(tipoVeloc,"u")){  // velocidade 50km/h
        *speedRound = 72;
    }

    else{
        if(volta == 0){  //velocidade 25km/h
            *speedRound = 144;
        }

        else {
            srand(time(NULL));

            if((rand() % 2) == 0)
                *speedRound = 144;

            else *speedRound = 72;
        }
    }
}

void *ciclista(void *a){
  int proxFaixa, numVolta;
  long ciclista_id = (long) a;
  int posAtual = posInic[ciclista_id];
  int speedRound = 0;
  int faixa = 0;

  configVelocidadeDaVolta(0, &speedRound);

  for(numVolta = 0; numVolta < 5;){
    //sem_wait(&mutex);

    proxFaixa = tentaAvancarMetro(posAtual, speedRound);

    if(proxFaixa != -1){
        posAtual++;

        sem_wait(&mutex);
        removaIdPosicaoAntiga(posAtual-1, faixa);
        sem_post(&mutex);

        if(posAtual == d){
            posAtual = 0;
            numVolta++;
            configVelocidadeDaVolta(numVolta, &speedRound);
        }

        sem_wait(&mutex);
        adicioneIdPosicaoAtual(posAtual, proxFaixa, ciclista_id);
        sem_post(&mutex);

        faixa = proxFaixa;
    }


    /** preciso:
        - chamar adiciona e remove ids na pista - ok
        - definir a velocidade para aquela volta do ciclista - ok
    **/

    printf("Thread: %ld;    # da Volta: %d;    Posicao: %d\n", ciclista_id, numVolta, posAtual);
    //sem_post(&mutex);
  }

  return NULL;
}

int main(int argc, char** argv)
{
    long i;
    pthread_t corredores[NUM_CICLISTA];

	parserEntrada(argc, argv);
	criaPista();
	geraLargada();
	imprimePista();
	printf("\n--------------------------------\n");

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

/************************************/
