#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "account.h"


Account accounts[MAX_ACCOUNTS];
int account_count = 0;
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

void create_account(int id, float balance) {
    pthread_mutex_lock(&account_mutex);
    accounts[account_count].id = id;
    accounts[account_count].balance = balance;
    account_count++;
    pthread_mutex_unlock(&account_mutex);
}

void deposit(int id, float amount) {
    usleep(100000); // Simula o tempo de processamento
    pthread_mutex_lock(&account_mutex);
    for (int i = 0; i < account_count; i++) {
        if (accounts[i].id == id) {
            accounts[i].balance += amount;
            break;
        }
    }
    pthread_mutex_unlock(&account_mutex);
}

void transfer(int from_id, int to_id, float amount) {
    usleep(200000); // Simula o tempo de processamento
    pthread_mutex_lock(&account_mutex);
    Account *from_account = NULL;
    Account *to_account = NULL;
    for (int i = 0; i < account_count; i++) {
        if (accounts[i].id == from_id) from_account = &accounts[i];
        if (accounts[i].id == to_id) to_account = &accounts[i];
    }
    if (from_account && to_account && from_account->balance >= amount) {
        from_account->balance -= amount;
        to_account->balance += amount;
    }
    pthread_mutex_unlock(&account_mutex);
}

void print_balance() {
    usleep(300000); // simula o tempo de processamento
    pthread_mutex_lock(&account_mutex);
    printf ("Balanço Total:\n");
    for (int i = 0; i < account_count; i++) {
        printf("Conta %d: %.2f\n", accounts[i].id, accounts[i].balance);
    }
    pthread_mutex_unlock(account_mutex);
}