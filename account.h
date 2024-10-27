#ifndef ACCOUNT_H
#define ACCOUNT_H

#define MAX_ACCOUNTS 10

typedef struct {
    int id;
    float balance;
} Account;

void create_account(int id, float balance);
void deposit(int id, float amount);
void transfer(int from_id, int to_id, float amount);
void print_balance();

#endif