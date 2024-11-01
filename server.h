#ifndef SERVER_H
#define SERVER_H

#include "account.h"

extern int POOL_SIZE;


void init_queue();
void enqueue(Request req);
Request dequeue();
void* server_thread_func(void* arg);

extern Account accounts[MAX_ACCOUNTS];
extern RequestQueue queue;
extern pthread_t thread_pool[POOL_SIZE];
extern pthread_t server_thread;
extern int stop_server;

#endif // SERVER_H
