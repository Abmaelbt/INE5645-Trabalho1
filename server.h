#ifndef SERVER_H
#define SERVER_H

typedef struct {
    int id;
    float balance;
} Account;

void* server_function(void *arg);
void* worker_function(void *arg);

#endif