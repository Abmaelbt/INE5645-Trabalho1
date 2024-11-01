#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "server.h"
#include "account.h"

#define MAX_ACCOUNTS 2
#define MAX_QUEUE_SIZE 20
#define POOL_SIZE 2 // numero de threads
#define MAX_REQUESTS 40 // maximo de requisicoes

// Contas e fila de requisições
Account accounts[MAX_ACCOUNTS];
RequestQueue queue;
pthread_t thread_pool[POOL_SIZE];
pthread_t server_thread;
int stop_server = 0;

// Função principal
int main() {
    init_accounts();
    init_queue();

    printf("Iniciando threads trabalhadoras...\n");
    int thread_ids[POOL_SIZE];
    for (int i = 0; i < POOL_SIZE; i++) {
        thread_ids[i] = i + 1; // Atribui IDs para as threads
        pthread_create(&thread_pool[i], NULL, worker_thread, &thread_ids[i]);
    }
    pthread_create(&server_thread, NULL, server_thread_func, NULL);

    pthread_join(server_thread, NULL);
    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_join(thread_pool[i], NULL);
    }

    // Limpeza
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }
    pthread_mutex_destroy(&queue.lock);
    pthread_cond_destroy(&queue.cond);

    printf("Todas as threads foram encerradas. Servidor finalizado.\n");
    return 0;
}
