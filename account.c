#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "account.h"

// Função para inicializar as contas
void init_accounts(Account accounts[], int max_accounts) {
    for (int i = 0; i < max_accounts; i++) {
        accounts[i].account_id = i + 1;
        accounts[i].balance = 1000.0; // Saldo inicial
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}
