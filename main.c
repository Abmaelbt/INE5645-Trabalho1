#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


#include "server.h"
#include "account.h"

Account accounts[MAX_ACCOUNTS];
RequestQueue queue;
pthread_t thread_pool[POOL_SIZE];
pthread_t server_thread;
int stop_server = 0;

// Função para inicializar as contas
void init_accounts() {
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        accounts[i].account_id = i + 1;
        accounts[i].balance = 1000.0; // Saldo inicial
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

// Função para inicializar a fila de requisições
void init_queue() {
    queue.front = 0;
    queue.rear = 0;
    queue.size = 0;
    pthread_mutex_init(&queue.lock, NULL);
    pthread_cond_init(&queue.cond, NULL);
}

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
