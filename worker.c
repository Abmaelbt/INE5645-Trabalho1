#include "server.h"
#include "account.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* worker_thread(void* arg) { 
    int thread_id = *((int*)arg);
    printf("Thread trabalhadora %d iniciada.\n", thread_id);
    while (!stop_server) {
        Request req = dequeue();
        
        if (req.operation == 1) { // Depósito
            deposit(req.src_account, req.amount, thread_id);

        } else if (req.operation == 2) { // Transferência
            transfer(req.src_account, req.dest_account, req.amount, thread_id);

        } else if (req.operation == 3) { // Balanço
            balance_report(thread_id);
        }
    }
    printf("Thread trabalhadora %d encerrando.\n", thread_id);
    return NULL;
}
