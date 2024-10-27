#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "account.h"
#include "server.h"
#include <time.h>

/*
#define NUM_CLIENTS 10
#define MAX_REQUESTS 100
#define POOL_SIZE 5
#define SERVICE_TIME 5

*/

int main() {
    create_account(1, 1000.0);
    create_account(2, 2000.0);

    pthread_t server_thread;
    Worker workers[MAX_WORKERS];
    for (int i = 0; i < MAX_WORKERS; i++) {
        workers[i].active = 0;
        pthread_mutex_init(&workers[i].lock, NULL);
        pthread_cond_init(&workers[i].cond, NULL);
        pthread_create(&workers[i].thread, NULL, worker_function, &workers[i]);
    }

    pthread_create(&server_thread, NULL, server_function, workers);

    pthread_join(server_thread, NULL);
    for (int i=0;i < MAX_WORKERS; i++) {
        pthread_join(workers[i].thread, NULL);
    }

    return 0;
}



