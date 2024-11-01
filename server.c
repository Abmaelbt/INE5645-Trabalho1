#include "server.h"
#include "account.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

Request request_queue[MAX_QUEUE_SIZE];
int queue_start = 0; 
int queue_end = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

// Recebido de main.c
extern int MAX_REQUESTS; 
extern int server_running; 

void add_request(Request req) {
    pthread_mutex_lock(&queue_mutex);
    request_queue[queue_end] = req;
    queue_end = (queue_end + 1) % MAX_QUEUE_SIZE;
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&queue_mutex);
}

void* server_function(void *arg) {
    struct { Worker* workers; int pool_size; }* args = arg;
    Worker* workers = args->workers;
    int pool_size = args->pool_size;
    int request_count = 0;

    printf("Servidor iniciado com %d threads no pool.\n", pool_size);
    
    while (request_count < MAX_REQUESTS) { //criterio de parada. Roda até atingir o numero maximo de requisições
        pthread_mutex_lock(&queue_mutex);
        while (queue_start == queue_end) {
            pthread_cond_wait(&queue_not_empty, &queue_mutex);
        }

        Request req = request_queue[queue_start];
        queue_start = (queue_start + 1) % MAX_QUEUE_SIZE;
        pthread_mutex_unlock(&queue_mutex);

        int worker_assigned = 0;
        for (int i = 0; i < pool_size; i++) {
            pthread_mutex_lock(&workers[i].lock);
            if (!workers[i].active) {
                workers[i].request = req;
                workers[i].active = 1;
                pthread_cond_signal(&workers[i].cond);
                printf("Request type %d processado por worker %d.\n", req.type, i);
                worker_assigned = 1;
                pthread_mutex_unlock(&workers[i].lock);
                break;
            }
            pthread_mutex_unlock(&workers[i].lock);
        }

        if (!worker_assigned) {
            printf("Nenhum worker disponível. Requisição será reprocessada.\n");
            add_request(req); // Recoloca a requisição na fila
        }

        request_count++;

        if (request_count % 10 == 0) {
            Request balance_request = {.type = GENERAL_BALANCE};
            add_request(balance_request);
        }
    }
    server_running = 0;
    printf("Servidor finalizou após processar %d requests.\n", request_count);
    return NULL;
}
