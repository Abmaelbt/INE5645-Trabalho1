#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 2
#define MAX_QUEUE_SIZE 20
#define POOL_SIZE 2 // numero de threads
#define MAX_REQUESTS 40 // maximo de requisicoes

typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock;
} Account;

typedef struct {
    int operation; // 1: Depósito, 2: Transferência, 3: Balanço
    int src_account; // Para operações de transferência
    int dest_account; // Para operações de transferência
    double amount;
} Request;

typedef struct {
    Request requests[MAX_QUEUE_SIZE];
    int front, rear, size;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} RequestQueue;

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

// Função para enfileirar uma requisição
void enqueue(Request req) {
    pthread_mutex_lock(&queue.lock);
    while (queue.size == MAX_QUEUE_SIZE) {
        pthread_cond_wait(&queue.cond, &queue.lock);
    }
    queue.requests[queue.rear] = req;
    queue.rear = (queue.rear + 1) % MAX_QUEUE_SIZE;
    queue.size++;
    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.lock);
}

// Função para desenfileirar uma requisição
Request dequeue() {
    Request req;
    pthread_mutex_lock(&queue.lock);
    while (queue.size == 0 && !stop_server) {
        pthread_cond_wait(&queue.cond, &queue.lock);
    }
    if (stop_server) {
        pthread_mutex_unlock(&queue.lock);
        exit(0);
    }
    req = queue.requests[queue.front];
    queue.front = (queue.front + 1) % MAX_QUEUE_SIZE;
    queue.size--;
    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.lock);
    return req;
}

// Função das threads trabalhadoras
void* worker_thread(void* arg) {
    int thread_id = *((int*)arg);
    printf("Thread trabalhadora %d iniciada.\n", thread_id);
    while (!stop_server) {
        Request req = dequeue();
        if (req.operation == 1) { // Depósito
            pthread_mutex_lock(&accounts[req.src_account - 1].lock);
            accounts[req.src_account - 1].balance += req.amount;
            printf("Thread %d: Depositou %.2f na conta %d. Novo saldo: %.2f\n", 
                   thread_id, req.amount, req.src_account, accounts[req.src_account - 1].balance);
            pthread_mutex_unlock(&accounts[req.src_account - 1].lock);
            usleep(500000); // Simula tempo de processamento
        } else if (req.operation == 2) { // Transferência
            pthread_mutex_lock(&accounts[req.src_account - 1].lock);
            pthread_mutex_lock(&accounts[req.dest_account - 1].lock);
            if (accounts[req.src_account - 1].balance >= req.amount) {
                accounts[req.src_account - 1].balance -= req.amount;
                accounts[req.dest_account - 1].balance += req.amount;
                printf("Thread %d: Transferiu %.2f da conta %d para %d. Novos saldos: %.2f, %.2f\n",
                       thread_id, req.amount, req.src_account, req.dest_account,
                       accounts[req.src_account - 1].balance, accounts[req.dest_account - 1].balance);
            } else {
                printf("Thread %d: Transferência falhou da conta %d: Saldo insuficiente.\n", 
                       thread_id, req.src_account);
            }
            pthread_mutex_unlock(&accounts[req.dest_account - 1].lock);
            pthread_mutex_unlock(&accounts[req.src_account - 1].lock);
            usleep(500000); // Simula tempo de processamento
        } else if (req.operation == 3) { // Balanço
            printf("Thread %d: Relatório de balanço:\n", thread_id);
            for (int i = 0; i < MAX_ACCOUNTS; i++) {
                pthread_mutex_lock(&accounts[i].lock);
                printf("Thread %d: Conta %d: %.2f\n", thread_id, accounts[i].account_id, accounts[i].balance);
                pthread_mutex_unlock(&accounts[i].lock);
            }
            usleep(500000); // Simula tempo de processamento
        }
    }
    printf("Thread trabalhadora %d encerrando.\n", thread_id);
    return NULL;
}

// Função da thread do servidor
void* server_thread_func(void* arg) {
    printf("Thread do servidor iniciada.\n");
    srand(time(NULL));
    for (int i = 0; i < MAX_REQUESTS; i++) { 
        Request req;
        req.operation = rand() % 2 + 1; // Seleciona aleatoriamente depósito ou transferência
        req.src_account = rand() % MAX_ACCOUNTS + 1;
        req.amount = (rand() % 200) - 100; // Quantia aleatória entre -100 e +100

        if (req.operation == 2) { // Transferência requer uma conta de destino
            req.dest_account = rand() % MAX_ACCOUNTS + 1;
            while (req.src_account == req.dest_account) { // Garante contas diferentes
                req.dest_account = rand() % MAX_ACCOUNTS + 1;
            }
        } else {
            req.dest_account = req.src_account; // Configura para a mesma conta nos depósitos
        }
        enqueue(req);

        if (i % 10 == 0) { // A cada 10 requisições, enfileira um relatório de balanço
            Request balance_req;
            balance_req.operation = 3;
            balance_req.src_account = -1;
            balance_req.dest_account = -1;
            balance_req.amount = 0;
            enqueue(balance_req);
        }

        usleep(100000); // Pausa para controlar a taxa de requisições
    }
    stop_server = 1; // Sinaliza para as threads pararem
    for (int i = 0; i < POOL_SIZE; i++) {
        enqueue((Request){.operation = 0}); // Enfileira sinal de parada
    }
    printf("Thread do servidor encerrando.\n");
    return NULL;
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
