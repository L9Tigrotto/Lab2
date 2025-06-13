#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>

#include <pthread.h>

#include "../shared/concurrent_queue.h"
#include "../shared/comunications.h"
#include "../shared/async.h"

#define MAX_SECRET 3000

typedef struct server_message_node_t {
    long int client_id;
    int secret;
    int number_of_servers;

    struct server_message_node_t* next;
} server_message_node_t;

typedef struct server_message_list_t {
    server_message_node_t* head;

    pthread_mutex_t mutex;
} server_message_list_t;

#define SERVER_MESSAGE_LIST_INITIALIZE_FAILURE 0
#define SERVER_MESSAGE_LIST_INITIALIZE_SUCCESS 1

int server_message_list_initialize(server_message_list_t* server_message_list_t);

#define SERVER_MESSAGE_LIST_DESTROY_FAILURE 0
#define SERVER_MESSAGE_LIST_DESTROY_SUCCESS 1

int server_message_list_destroy(server_message_list_t* server_message_list_t);

#define SERVER_MESSAGE_LIST_INSERT_FAILURE 0
#define SERVER_MESSAGE_LIST_INSERT_SUCCESS 1

int server_message_list_insert(server_message_list_t* server_message_list_t, long int client_id, int secret);
