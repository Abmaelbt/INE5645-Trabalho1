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
        printf("Conta %d criada com saldo inicial: %.2f\n", id, balance);
        account_count++;
        pthread_mutex_unlock(&account_mutex);
    }

    // funcao para deposito
    // recebe um identificador de conta (um número inteiro positivo) e o valor de depósito (um número real, que pode ser positivo ou negativo).  
    // Note que esta operação pode ser executada tanto para depósitos quanto saques, dependendo se o valor de depósito é positivo ou negativo;
    void deposit(int id_deposito, float valor_deposito) {
        usleep(500000); // Simula o tempo de processamento
        pthread_mutex_lock(&account_mutex);  // Protege a operação na conta

        for (int i = 0; i < account_count; i++) {
            if (accounts[i].id == id_deposito) {
                // Verifica se é saque e se há saldo suficiente
                if (valor_deposito < 0 && accounts[i].balance + valor_deposito < 0) {
                    printf("Saque falhou: saldo insuficiente para a conta %d.\n", id_deposito);
                } else {
                    accounts[i].balance += valor_deposito;
                    if (valor_deposito >= 0) {
                        printf("Depositado: %.2f na conta: %d\n", valor_deposito, id_deposito);
                    } else {
                        printf("Sacado: %.2f da conta: %d\n", -valor_deposito, id_deposito);
                    }
                }
                break;
            }
        }

        pthread_mutex_unlock(&account_mutex); // Libera o mutex após operação
    }

    // funcao para transferencia
    // Transferência entre contas: Dadas duas contas bancárias, origem e destino, e um
    // valor de transferencia, esta operação deve debitar o valor de transferência da conta
    // de origem e somar este valor na conta destino;
    void transfer(int origem_id, int destino_id, float valor_transferencia) {
        usleep(100000); // Simula o tempo de processamento
        pthread_mutex_lock(&account_mutex);  // Protege a operacao nas contas

        if (origem_id == destino_id) {
            printf("Transferência falhou: origem e destino são iguais para a conta %d.\n", origem_id);
        } else {
            Account *origem = NULL, *destino = NULL;

            for (int i = 0; i < account_count; i++) {
                if (accounts[i].id == origem_id) origem = &accounts[i];
                if (accounts[i].id == destino_id) destino = &accounts[i];
            }

            if (origem && destino) {
                if (origem->balance >= valor_transferencia) {
                    origem->balance -= valor_transferencia;
                    destino->balance += valor_transferencia;
                    printf("Transferência de %.2f da conta %d para a conta %d. Saldos atualizados: %.2f (origem), %.2f (destino)\n",
                        valor_transferencia, origem_id, destino_id, origem->balance, destino->balance);
                } else {
                    printf("Transferência falhou: saldo insuficiente na conta %d.\n", origem_id);
                }
            } else {
                printf("Transferência falhou: conta de origem ou destino não encontrada.\n");
            }
        }

        pthread_mutex_unlock(&account_mutex); // libera o mutex apos operacao
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