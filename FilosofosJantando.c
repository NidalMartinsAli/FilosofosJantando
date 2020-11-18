#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define ABERTO 1        //Id para estado semaforo
 
typedef struct nfilosofos{      //Estrutura de dados dos filósofos
        int quantidadeF;              //Quantidade de filósofos
        int quantidadeM;            //Quantidade de macarrão
        int id;                 //Índice do filósofo
        
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
//  void mostrar (int i);
//  void teste (int i,int quantidade);



//inicialmente cria a alocação dos semaforos, mutex, threads,
// logo parte distribui as threads partindo para a recursão
// ao final, destroi todos semaforos, threads, mutex e liberação de memória.
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
      
            vetorFilo[i].id = i;               //indice do filosofo
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
//O filosofo pensa por um tempo randomico, ou melhor
// a thread fica adormecida por um tempo X em segundos ou milissegundos
// quanto maior for o tempo, menos chance de ocorrencia de starvation 
// Cada segundo representa uma unidade de comida, ou seja, se esperar 5 segundos come 5 unidades de macarrão
void pensar(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    int tempo;
    tempo=(rand() % 5+1);           //tempo para pensar

    printf("\nfilosofo %d a PENSAR por %ds", Filo->id,tempo);

    usleep(tempo*1000000);          //deixa o filosofo pensando por alguns segundos
    Filo->quantidadeM = tempo; //quantidade de macarrao que irá comer na proxima vez

    esperar(F);                 //inicia a próxima ação
}



//Cria a ação comer
//Espera o sinal do semáforo, logo, bloqueia o semáforo e o mutex
//para entrar na seção critica do macarrão...
//verifica se a quantidade de macarrão é negativa, ( não termina execução caso seja ...
//Pois só deve acabar de executar no proximo filosofo que tentar comer, ou seja, um filosofo
//come tudo, o proximo a tentar comer verifica que não tem macarrão e termina o programa.)
// Ao terminar de comer, o filosofo desbloqueia o semaforo e mutex, largando também os garfos.
void comer(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    sem_wait(&(macarrao));      //espera o semaforo abrir para comer macarrao
    pthread_mutex_lock(&(m));   //bloqueia a seção critica do macarrao
    
    if(qMacarrao <=0){          //verifica se ainda há macarrão
        printf("\nFilosofo %d foi tentar COMER %d, mas o Macarrao acabou\n",Filo->id,Filo->quantidadeM);
        exit(1);
    }
    qMacarrao = qMacarrao- Filo->quantidadeM;   // decrementa do macarrão
    if(qMacarrao<=0){           //para não ter macarrão negativo
        qMacarrao=0;
    }
    printf("\nFilosofo %d COMEU %d, Macarrao restante %d",Filo->id,Filo->quantidadeM,qMacarrao);
    pthread_mutex_unlock(&(m)); //desbloqueia o macarrao
    sem_post(&(macarrao));      //libera o semaforo 

    //larga os garfos
    larga_garfos(F);

    pensar(F);
}


//ação de espera
//os IFS concatenados referem-se a verificação do ultimo elemento no vetor de garfos (caso
//seja o ultimo filosofo, deve pegar o garfo 0 e ultimo)

//tenta pegar 2 garfos verificando o semaforo e vai para função comer
//caso não consiga pegar 2 garfos, não pega nenhum e fica esperando 1 microsegundo
void esperar(void *F){
    NFilosofos *Filo = (NFilosofos*) F;
    int aux1,aux2;                            //auxiliar que armazena o estado do semaforo

    if(Filo->id==Filo->quantidadeF-1){          //verifica se está no limite do vetor
        sem_getvalue(&(garfo[Filo->id]),&aux1); //armazena em aux o estado do semaforo
        sem_getvalue(&(garfo[0]),&aux2);        //armazena em aux o estado do semaforo

        if((aux1)==1 &&  (aux2)==1){                 //verifica se tem 2 garfos disponiveis
            pega_garfos_limite(F);           
            comer(F);
        }
        else { 
            if((aux1)==1 &&  (aux2)!=1){                                   //se não houver 1 ou 0 garfos disponiveis espera 1s
                printf("\nFilosofo %d o garfo da DIREITA está sendo usado, ESPERANDO liberação de ambos", Filo->id);
                usleep(1);
                esperar(F);
            }
            if(((aux1)!=1 &&  (aux2)==1)){
                printf("\nFilosofo %d o garfo da ESQUERDA está sendo usado, ESPERANDO liberação de ambos", Filo->id);
                usleep(1);
                esperar(F);
            }
            else{
                printf("\nFilosofo %d ESPERANDO os 2 garfos", Filo->id);
                usleep(1);
                esperar(F);
            }
        }
    }
    else{
        sem_getvalue(&(garfo[Filo->id]),&aux1); //armazena em aux o estado do semaforo
        sem_getvalue(&(garfo[0]),&aux2);        //armazena em aux o estado do semaforo

        if(aux1==1 && aux2==1){                 //verifica se tem 2 garfos disponiveis
            pega_garfos(F);
            comer(F);
        }
        else { 
            if((aux1)==1 &&  (aux2)!=1){                                   //se não houver 1 ou 0 garfos disponiveis espera 1s
                printf("\nFilosofo %d o garfo da DIREITA está sendo usado, ESPERANDO liberação de ambos", Filo->id);
                usleep(1);
                esperar(F);
            }
            if(((aux1)!=1 &&  (aux2)==1)){
                printf("\nFilosofo %d o garfo da ESQUERDA está sendo usado, ESPERANDO liberação de ambos", Filo->id);
                usleep(1);
                esperar(F);
            }
            else{
                printf("\nFilosofo %d ESPERANDO os 2 garfos", Filo->id);
                usleep(1);
                esperar(F);
            }
        }
    }

    
}


//pega os garfos do inicio ao meio do vetor, bloqueia o semaforo e mutex de garfos
void pega_garfos(void *F){
    NFilosofos *Filo = (NFilosofos*) F;

    sem_wait(&(garfo[Filo->id]));       //bloqueia o garfo da esquerda
    sem_wait(&(garfo[Filo->id+1]));     //bloqueia o garfo da direita

    //entra na seção critica dos garfos \/
    pthread_mutex_lock(&(g[Filo->id]));
    pthread_mutex_lock(&(g[Filo->id+1]));
    comer(F);
}

//pega os garfos do final do vetor filosofo, também bloqueia a seção critica do garfo 
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
// desbloqueia o garfo, dando passagem para um próximo.
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









