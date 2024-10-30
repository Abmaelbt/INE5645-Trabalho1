#include "server.h"
#include "account.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


// implementa a lista circular para armazenar as requisicoes
Request request_queue[MAX_QUEUE_SIZE];
int queue_start = 0; 
int queue_end = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;


void add_request(Request req) {
    pthread_mutex_lock(&queue_mutex);
    request_queue[queue_end] = req; // queue_end representa o indice onde a proxima requisição vai ser inserida
    queue_end = (queue_end + 1) % MAX_QUEUE_SIZE; // faz com que queue_end volte ao inicio [0] quando atinge o final da fila (em uma fila circular).
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&queue_mutex);
}

void* server_function(void *arg) {
    Worker *workers = (Worker*) arg;
    int request_count = 0;

    while (1) { // loop infinito
        pthread_mutex_lock(&queue_mutex);
        while (queue_start == queue_end) pthread_cond_wait(&queue_not_empty, &queue_mutex);
        Request req = request_queue[queue_start];
        queue_start = (queue_start + 1) % MAX_QUEUE_SIZE;
        pthread_mutex_unlock(&queue_mutex);

        // Despacha a requisição para uma thread trabalhadora livre
        for (int i = 0;i < MAX_WORKERS; i++) {
            pthread_mutex_lock(&workers[i].lock);
            if (!workers[i].active) {
                workers[i].request = req;
                workers[i].active = 1;
                pthread_cond_signal(&workers[i].cond);
                pthread_mutex_unlock(&workers[i].lock);
                break;
            }
            pthread_mutex_unlock(&workers[i].lock);
        }
    
        // a cada 10 operações de clientes, o servidor adiciona uma operação de balanço geral (c) na fila de requisições.
        request_count++;
        if (request_count % 10 == 0) {
            Request balance_request = {.type = GENERAL_BALANCE};
            add_request(balance_request);
        }

    }
    
}