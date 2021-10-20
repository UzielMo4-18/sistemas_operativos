#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define THREAD_NUM 2

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

int buffer[10];
int count = 0;

void* producer(void* args){
    while(1){
        int x = rand() % 100;
        sleep(1);

        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        buffer[count] = x;
        count++;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }
}

void* consumer(void* args){
    while(1){
        int y;

        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        y = buffer[count - 1];
        count--;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);

        printf("Obtuvo %d\n", y);
        sleep(1);
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);
    int i;
    for (i = 0 ;i < THREAD_NUM; i++){
        if(i % 2 == 0){
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
    return 0;
}