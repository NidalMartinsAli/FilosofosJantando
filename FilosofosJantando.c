#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
  
#define PENSAR 0        //Id para estado pensando
#define ESPERAR 1       //Id para estado esperando
#define COMER 2         //Id para estado comendo
 
typedef struct nfilosofos{      //Estrutura de dados dos filósofos
        int quantidadeF;              //Quantidade de filósofos
        int quantidadeM;			//Quantidade de macarrão
        int id;                 //Índice do filósofo
        int tempoPensar;		// Tempo de cada filosofo para pensar
      	int estado;				// estado em que se encontra o filosofo
      	
}NFilosofos;

sem_t macarrao;        //Representa a comida que os filosofos irão comer
sem_t *garfo;           //Vetor que representa os garfos
 
 	void *filosofo (void *F);
//	void comer (NFilosofos F);
//	void esperar (NFilosofos F);
	void pensar (int id);
//	void mostrar (int i);
//	void teste (int i,int quantidade);


void main (){
	int qFilo, qMacarrao, i;
	NFilosofos *vetorFilo; //vetor de structs Nfilosofos
	pthread_t *thFilo; //vetor de threads dos filosofos
	
		printf("Insira a quantidade de filósofos que irão comer. ");
		scanf("%d", &qFilo);
		
		if(qFilo<1){
			printf("ERRO! Não há filósofos o suficiente.");
			return;
		}
		
		printf("Insira a quantidade de macarrão a ser comida pelos filósofos.");
		scanf("%d", &qMacarrao);
		
		if(qMacarrao<1){
			printf("ERRO! Não há comida o suficiente.");
			return;
		}

		vetorFilo = (NFilosofos*) malloc ((qFilo)*sizeof(NFilosofos));  //Aloca vetor de filósofos
        thFilo = (pthread_t*) malloc ((qFilo)*sizeof(pthread_t));   //Aloca vetor de threads
        garfo = (sem_t*)malloc((qFilo)*sizeof(sem_t));    //Aloca a quantidade de garfos = quantidade de filósofos

        sem_init(&macarrao, 0, qMacarrao); //semaforo do macarrao
        
        for (i=0; i<qFilo; i++){
        	 sem_init(&garfo[i], 0, 2); //semaforo dos garfos
        }

        for (i=0;i<qFilo;i++){         //Inicializa o vetor com os dados dos filósofos
            vetorFilo[i].quantidadeF = qFilo;
            vetorFilo[i].tempoPensar = rand() % 1000;  //tempo aleatório para pensar
      
       	    vetorFilo[i].id = i;		//indice do filosofo
            vetorFilo[i].estado = PENSAR;         //cada filosofo inicia no estado pensar
            pthread_create (&thFilo[i],NULL,filosofo,&vetorFilo[i]);   //Cria as threads filósofos
	}

      	for (i=0;i<qFilo;i++){            //Faz um join nos filósofos
            pthread_join (thFilo[i],NULL);
        }

        sem_destroy(&macarrao);
        
        for (i=0;i<qFilo;i++){
                sem_destroy(&garfo[i]);
        }
 
        free(garfo);
        free(vetorFilo);
        free(thFilo);
        pthread_exit(NULL);
}


//função destinada a criação da thread filosofo e ao ato de pensar inicial
void *filosofo(void *F){
	 
	 NFilosofos Filo = *(NFilosofos*) F;
       
        while (1){
                pensar(Filo.id);     
        }
}


//cria o pensamento do filosofo
void pensar(int id){

    int tempo;
    tempo=(rand() % 5+1);
    usleep(tempo*1000000);          //deixa o filosofo pensando por alguns milissegundos
         
    printf("\nfilosofo %d a pensar por %ds", id,tempo);


}






        
















