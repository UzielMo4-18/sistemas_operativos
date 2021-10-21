#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define THREAD_NUM 5

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;
pthread_mutex_t mutexA;

int buffer[10];
int count = 0, nc = 0, npe = 0;
bool escribiendo = false;

void* producer(void* args){
    while(1){
        int x = rand() % 100;
        sleep(1);
        pthread_mutex_lock(&mutexA);
        npe++;
        printf("Numero de productores en espera->%d",npe);
        while (nc > 0 || escribiendo)
        {
            pthread_mutex_unlock(&mutexA);
            pthread_mutex_lock(&mutexA);
        }
        
        //sem_wait(&semEmpty);
        escribiendo = true;
        npe--;
        pthread_mutex_unlock(&mutexA);
        //pthread_mutex_lock(&mutexBuffer);
        buffer[count] = x;
        count++;
        pthread_mutex_lock(&mutexA);
        //pthread_mutex_unlock(&mutexBuffer);
        escribiendo = false;
        pthread_mutex_unlock(&mutexA);
        //sem_post(&semFull);
    }
}

void* consumer(void* args){
    while(1){
        int y;
        pthread_mutex_lock(&mutexA);
        //sem_wait(&semFull);
        while (npe > 0 || escribiendo)
        {
            pthread_mutex_unlock(&mutexA);
            pthread_mutex_lock(&mutexA);
        }
        
        nc++;
        printf("\nNumero de consumidores ->%d",nc);
        pthread_mutex_unlock(&mutexA);
        //pthread_mutex_lock(&mutexBuffer);
        y = buffer[count - 1];
        count--;
        //pthread_mutex_unlock(&mutexBuffer);
        nc--;
        //sem_post(&semEmpty);

        printf("\nObtuvo %d\n", y);
        sleep(1);
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_mutex_init(&mutexA, NULL);
    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);
    int i;
    for (i = 0 ;i < THREAD_NUM; i++){
        if(i % 2 != 0){
            if(pthread_create(&th[i], NULL, &producer, NULL) != 0) perror("Creación de hilo fallida");
        }
        else{
            if(pthread_create(&th[i], NULL, &consumer, NULL) != 0) perror("Creación de hilo fallida");
        }
    }
    for (i = 0; i < THREAD_NUM; i++){
        if(pthread_join(th[i], NULL) != 0) perror("Union de hilos fallida");
    }
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    pthread_mutex_destroy(&mutexA);
    return 0;
}