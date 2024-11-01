#include "account.h"
#include <stdio.h>
#include <pthread.h>
#include "server.h"

static void init_accounts() { // Make init_accounts static
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        accounts[i].account_id = i + 1;
        accounts[i].balance = 1000.0; // Saldo inicial
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}
