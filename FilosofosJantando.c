#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
  
#define PENSAR 0        //Id para estado pensando
#define ESPERAR 1       //Id para estado esperando
#define COMER 2         //Id para estado comendo

 typedef struct nfilosofos{      //Estrutura de dados dos filósofos
        //int quantidadeF;              //Quantidade de filósofos \\ é necessário? pode causar desperdicio de memória
        int quantidadeM;			  //Quantidade de macarrão
        int id;                 	  //Índice do filósofo
        int tempoPensar;			 //tempo que o filóso tem para pensar

int *estado;            //Vetor que armazena os estados de cada thread
sem_t macarrao;        //Representa a comida que os filosofos irão comer
sem_t *garfo;           //Vetor que representa os garfos
 
/* 	void *filosofo (void *F);
	void comer (NFilosofos F);
	void esperar (NFilosofos F);
	void pensar (NFilosofos F);
	void mostrar (int i);
	void teste (int i,int quantidade);
*/

void main (){
	int qFilo, qMacarrao;
	NFilosofos *vetorFilo; //vetor de structs Nfilosofos
	pthread_t *thFilo; //vetor de threads dos filosofos
	
		printf("Insira a quantidade de filósofos que irão comer. ");
		scanf("%d", &qFilo);
		
		if(qFilo<1){
			printf("ERRO! Não há filósofos o suficiente.");
			break;
		}
		
		printf("Insira a quantidade de macarrão a ser comida pelos filósofos.");
		scanf("%d", &qMacarrao);
		
		if(qMacarrao<1){
			printf("ERRO! Não há comida o suficiente.");
			break;
		}


        vetorFilo = (NFilosofos*) malloc ((qFilo)*sizeof(NFilosofos));  //Aloca vetor de filósofos
        thFilo = (pthread_t*) malloc ((qFilo)*sizeof(pthread_t));   //Aloca vetor de threads
        garfo = (sem_t*)malloc((qFilo)*sizeof(sem_t));    //Aloca a quantidade de garfos = quantidade de filósofos
        macarrao = (sem_t*)malloc((qMacarrao)*sizeof(sem_t)); //Aloca a quantidade de macarrão
        estado = (int*)malloc((qFilo)*sizeof(int));     //Aloca o vetor de estado dos filósofos	

        sem_init(&macarrao, 0, qMacarrao); //semaforo do macarrao
        
        for (int i=0, i<qFilo; i++){
        	 sem_init(&macarrao[i], 0, 2); //semaforo dos garfos
        }



















}
  
