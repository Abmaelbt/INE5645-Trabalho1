#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 10
#define MAX_REQUESTS 100
#define POOL_SIZE 5

// Structure to hold account information
typedef struct {
    int id;
    double balance;
    pthread_mutex_t lock;  // Mutex for protecting account operations
} Account;

// Request types
typedef enum { DEPOSIT, TRANSFER, BALANCE } RequestType;

// Structure for a bank operation request
typedef struct {
    RequestType type;
    int account_id;
    int target_account_id; // for transfer
    double amount;
} Request;

// Global Variables
Account accounts[NUM_ACCOUNTS];         // Array of accounts
Request request_queue[MAX_REQUESTS];    // Request queue
int queue_size = 0;                     // Current size of the request queue
pthread_mutex_t queue_mutex;            // Mutex to protect the request queue
pthread_cond_t queue_cond;              // Condition variable for queue signaling

// Function to initialize accounts
void init_accounts() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].id = i;
        accounts[i].balance = 1000.0; // Initial balance for all accounts
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

// Function to perform deposit/withdrawal
void deposit(int account_id, double amount) {
    usleep(500000); // Simulating processing time
    pthread_mutex_lock(&accounts[account_id].lock);
    accounts[account_id].balance += amount;
    printf("Account %d: Deposit of %.2f, New Balance: %.2f\n", account_id, amount, accounts[account_id].balance);
    pthread_mutex_unlock(&accounts[account_id].lock);
}

// Function to transfer between accounts
void transfer(int src_account_id, int dest_account_id, double amount) {
    usleep(700000); // Simulating processing time
    pthread_mutex_lock(&accounts[src_account_id].lock);
    pthread_mutex_lock(&accounts[dest_account_id].lock);
    
    if (accounts[src_account_id].balance >= amount) {
        accounts[src_account_id].balance -= amount;
        accounts[dest_account_id].balance += amount;
        printf("Transfer: %.2f from Account %d to Account %d\n", amount, src_account_id, dest_account_id);
    } else {
        printf("Transfer failed: Insufficient funds in Account %d\n", src_account_id);
    }
    
    pthread_mutex_unlock(&accounts[dest_account_id].lock);
    pthread_mutex_unlock(&accounts[src_account_id].lock);
}

// Function to print the balance of all accounts
void general_balance() {
    usleep(400000); // Simulating processing time
    printf("\n--- General Balance Snapshot ---\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_lock(&accounts[i].lock);
        printf("Account %d: Balance: %.2f\n", accounts[i].id, accounts[i].balance);
        pthread_mutex_unlock(&accounts[i].lock);
    }
    printf("--------------------------------\n\n");
}

// Worker Thread Function
void *worker_thread(void *arg) {
    while (1) {
        Request req;
        
        // Wait for a request
        pthread_mutex_lock(&queue_mutex);
        while (queue_size == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        
        // Fetch the request from the queue
        req = request_queue[--queue_size];
        pthread_mutex_unlock(&queue_mutex);
        
        // Process the request
        if (req.type == DEPOSIT) {
            deposit(req.account_id, req.amount);
        } else if (req.type == TRANSFER) {
            transfer(req.account_id, req.target_account_id, req.amount);
        } else if (req.type == BALANCE) {
            general_balance();
        }
    }
    return NULL;
}

// Server Function (Main Thread)
void *server_thread(void *arg) {
    int processed_requests = 0;
    
    while (1) {
        // Simulate adding requests to the queue
        usleep(300000); // Simulate time between requests

        Request new_request;
        if (processed_requests % 10 == 0) {
            // Every 10th request is a general balance check
            new_request.type = BALANCE;
        } else {
            // Randomly generate deposit or transfer operations
            new_request.type = rand() % 2; // 0 for DEPOSIT, 1 for TRANSFER
            new_request.account_id = rand() % NUM_ACCOUNTS;
            new_request.amount = ((rand() % 200) - 100); // Amount between -100 and +100

            if (new_request.type == TRANSFER) {
                new_request.target_account_id = rand() % NUM_ACCOUNTS;
            }
        }

        // Add the request to the queue
        pthread_mutex_lock(&queue_mutex);
        request_queue[queue_size++] = new_request;
        pthread_cond_signal(&queue_cond); // Notify worker threads
        pthread_mutex_unlock(&queue_mutex);
        
        processed_requests++;
    }
    return NULL;
}

// Client Threads
void *client_thread(void *arg) {
    while (1) {
        usleep(1000000); // Simulate delay between client requests
        
        Request new_request;
        new_request.type = DEPOSIT; // Simulate deposit
        new_request.account_id = rand() % NUM_ACCOUNTS;
        new_request.amount = ((rand() % 200) - 100); // Random amount between -100 and 100
        
        // Add the request to the server
        pthread_mutex_lock(&queue_mutex);
        request_queue[queue_size++] = new_request;
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&queue_mutex);
    }
    return NULL;
}

// Main Function
int main() {
    srand(time(NULL));
    
    // Initialize accounts and synchronization structures
    init_accounts();
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);

    // Create worker thread pool
    pthread_t worker_threads[POOL_SIZE];
    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_create(&worker_threads[i], NULL, worker_thread, NULL);
    }

    // Create server thread
    pthread_t server;
    pthread_create(&server, NULL, server_thread, NULL);

    // Create some client threads
    pthread_t clients[3];
    for (int i = 0; i < 3; i++) {
        pthread_create(&clients[i], NULL, client_thread, NULL);
    }

    // Join threads (This simulates continuous operation, you can add a termination condition)
    for (int i = 0; i < 3; i++) {
        pthread_join(clients[i], NULL);
    }
    pthread_join(server, NULL);

    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_join(worker_threads[i], NULL);
    }

    // Clean up
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);

    return 0;
}
