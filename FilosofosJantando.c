#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#define FECHADO 0       //Id para estado semaforo
#define ABERTO 1        //Id para estado semaforo

  
#define PENSAR 0        //Id para estado pensando
#define ESPERAR 1       //Id para estado esperando
#define COMER 2         //Id para estado comendo
 
typedef struct nfilosofos{      //Estrutura de dados dos filósofos
        int quantidadeF;              //Quantidade de filósofos
        int quantidadeM;			//Quantidade de macarrão
        int id;                 //Índice do filósofo
      	int estado;				// estado em que se encontra o filosofo
      	
}NFilosofos;

int qMacarrao;          // quantidade de macarrão no prato central
sem_t macarrao;        //Representa a comida que os filosofos irão comer
sem_t *garfo;           //Vetor que representa os garfos
pthread_mutex_t m, *g;  //mutex para os garfos e macarrão

 	void *filosofo (void *F);
	void comer (void *F);
	void esperar (void *F);
	void pensar (void *F);
    void pega_garfos_limite(void *F);
    void pega_garfos(void *F);
    void larga_garfos(void *F);
//	void mostrar (int i);
//	void teste (int i,int quantidade);


void main (){
	int qFilo, i;
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
        g = (pthread_mutex_t*) malloc ((qFilo)*sizeof(pthread_mutex_t)); //Aloca a quantidade de mutex para o garfo
        
        for (i=0; i<qFilo; i++){
             pthread_mutex_init(&g[i], NULL);  //mutex dos garfos
        }

        pthread_mutex_init(&m, NULL);          //mutex do macarrao
        sem_init(&macarrao, 0, ABERTO);        //semaforo do macarrao
        
        for (i=0; i<qFilo; i++){
        	 sem_init(&garfo[i], 0, ABERTO);   //semaforo dos garfos
        }

        for (i=0;i<qFilo;i++){                 //Inicializa o vetor com os dados dos filósofos
            vetorFilo[i].quantidadeF = qFilo;
      
       	    vetorFilo[i].id = i;		       //indice do filosofo
            vetorFilo[i].estado = PENSAR;      //cada filosofo inicia no estado pensar
            pthread_create (&thFilo[i],NULL,filosofo,&vetorFilo[i]);   //Cria as threads filósofos
	}

      	for (i=0;i<qFilo;i++){            //Faz um join nos filósofos
            pthread_join (thFilo[i],NULL);
        }

        sem_destroy(&macarrao);
        
        for (i=0;i<qFilo;i++){
                sem_destroy(&garfo[i]);
        }
        
        pthread_mutex_destroy(&m);

        for (i=0;i<qFilo;i++){
                pthread_mutex_destroy(&g[i]);
        }
        free(garfo);
        free(vetorFilo);
        free(thFilo);
        pthread_exit(NULL);
}


//função cabeça que inicia a recursividade
void *filosofo(void *F){
	 
	NFilosofos Filo = *(NFilosofos*) F;
    
    pensar(F); 
        
}


//cria o pensamento do filosofo
void pensar(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    int tempo;
    tempo=(rand() % 5+1);           //tempo para pensar

    printf("\nfilosofo %d a pensar por %ds", Filo->id,tempo);

    usleep(tempo*1000000);          //deixa o filosofo pensando por alguns milissegundos
    Filo->quantidadeM = tempo; //quantidade de macarrao que irá comer na proxima vez

    
    esperar(F);
}

void comer(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    sem_wait(&(macarrao));      //espera o semaforo abrir para comer macarrao
    pthread_mutex_lock(&(m));   //bloqueia a seção critica do macarrao
    
    if(qMacarrao <=0){          //verifica se ainda há macarrão
        printf("\nFilosofo %d foi tentar comer %d, mas Macarrao acabou",Filo->id,Filo->quantidadeM);
        exit(1);
    }
    qMacarrao = qMacarrao- Filo->quantidadeM;   // decrementa do macarrão
    if(qMacarrao<=0){           //para não ter macarrão negativo
        qMacarrao=0;
    }
    printf("\nFilosofo %d comeu %d Macarrao total %d",Filo->id,Filo->quantidadeM,qMacarrao);
    pthread_mutex_unlock(&(m)); //desbloqueia o macarrao
    sem_post(&(macarrao));      //libera o semaforo 

    //larga os garfos
    larga_garfos(F);

    pensar(F);
}



//tenta pegar 2 garfos verificando o semaforo e vai para função comer
//caso não consiga pegar 2 garfos, fica esperando 1s
void esperar(void *F){
    NFilosofos *Filo = (NFilosofos*) F;
    int *aux1,*aux2;                            //auxiliar que armazena o estado do semaforo

    if(Filo->id==Filo->quantidadeF-1){          //verifica se está no limite do vetor
        sem_getvalue(&(garfo[Filo->id]),&aux1); //armazena em aux o estado do semaforo
        sem_getvalue(&(garfo[0]),&aux2);        //armazena em aux o estado do semaforo

        if(aux1==1 && aux2==1){                 //verifica se tem 2 garfos disponiveis
            pega_garfos_limite(F);           
            comer(F);
        }
        else{                                   //se não houver 1 ou 0 garfos disponiveis espera 1s
            printf("\nFilosofo %d ESPERANDO 2 GARFOS", Filo->id);
            usleep(1*1000000);                  //dorme por 1s
            esperar(F);
        }
    }
    else{
        sem_getvalue(&(garfo[Filo->id]),&aux1); //armazena em aux o estado do semaforo
        sem_getvalue(&(garfo[0]),&aux2);        //armazena em aux o estado do semaforo

        if(aux1==1 && aux2==1){                 //verifica se tem 2 garfos disponiveis
            pega_garfos(F);
            comer(F);
        }
        else{                                   //se não houver 1 ou 0 garfos disponiveis espera 1s
            printf("\nFilosofo %d ESPERANDO 2 GARFOS", Filo->id);
            usleep(1*1000000);                  //dorme por 1s
            esperar(F); 
            
        }
    }

    
}


//pega os garfos do inicio ao meio do vetor 
void pega_garfos(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    sem_wait(&(garfo[Filo->id]));       //bloqueia o garfo da esquerda
    sem_wait(&(garfo[Filo->id+1]));     //bloqueia o garfo da direita

    //entra na seção critica dos garfos \/
    pthread_mutex_lock(&(g[Filo->id]));
    pthread_mutex_lock(&(g[Filo->id+1]));
    comer(F);
}

//pega os garfos do final do vetor filosofo
void pega_garfos_limite(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    sem_wait(&(garfo[Filo->id]));       //bloqueia o garfo da esquerda
    sem_wait(&(garfo[0]));              //bloqueia o garfo da direita

    //entra na seção critica dos garfos \/
    pthread_mutex_lock(&(g[Filo->id])); 
    pthread_mutex_lock(&(g[0]));
}
        

//larga os garfos, mas primeiro verifica se o filosofo está no limite do vetor,
//caso esteja, pega o ultimo elemento + o primeiro do vetor garfos.
void larga_garfos(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    pthread_mutex_unlock(&(g[Filo->id]));   //larga o garfo da esquerda
    sem_post(&(garfo[Filo->id]));           //libera o semaforo da esquerda

    if(Filo->id==Filo->quantidadeF-1){      //verifica se é o ultimo elemento do vetor

        pthread_mutex_unlock(&(g[0]));      //larga o garfo da direita 
        sem_post(&(garfo[0]));              //libera o semaforo da direita
    }
    else{
        pthread_mutex_unlock(&(g[Filo->id+1])); //larga o garfo da direita
        sem_post(&(garfo[Filo->id+1]));         //libera o garfo da direita
    }
}









