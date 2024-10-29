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

// funcao para deposito
// recebe um identificador de conta (um número inteiro positivo) e o valor de depósito (um número real, que pode ser positivo ou negativo).  
// Note que esta operação pode ser executada tanto para depósitos quanto saques, dependendo se o valor de depósito é positivo ou negativo;
void deposit(int id_deposito, float valor_deposito) {

}

// funcao para transferencia
// Transferência entre contas: Dadas duas contas bancárias, origem e destino, e um
// valor de transferência, esta operação deve debitar o valor de transferência da conta
// de origem e somar este valor na conta destino;
void transfer(int conta_origem, int conta_destino, float valor_transferencia) {

}

void print_balance() {
    usleep(300000); // simula o tempo de processamento
    pthread_mutex_lock(&account_mutex);
    printf ("Balanço Total:\n");
    for (int i = 0; i < account_count; i++) {
        printf("Conta %d: %.2f\n", accounts[i].id, accounts[i].balance);
    }
    pthread_mutex_unlock(&account_mutex);
}