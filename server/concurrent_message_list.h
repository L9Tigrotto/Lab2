#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>

#include <pthread.h>

#include "../shared/concurrent_queue.h"
#include "../shared/comunications.h"
#include "../shared/async.h"

#define MAX_SECRET 3000

typedef struct client_message_node_t {
    int file_descriptor;
    long int client_id;
    
    struct timespec last_time;
    int secret;

    struct client_message_node_t* next;
} client_message_node_t;

typedef struct client_message_list_t {
    client_message_node_t* head;

    pthread_mutex_t mutex;
} client_message_list_t;

#define CLIENT_MESSAGE_LIST_INITIALIZE_FAILURE 0
#define CLIENT_MESSAGE_LIST_INITIALIZE_SUCCESS 1

int client_message_list_initialize(client_message_list_t* client_message_list);

#define CLIENT_MESSAGE_LIST_DESTROY_FAILURE 0
#define CLIENT_MESSAGE_LIST_DESTROY_SUCCESS 1

int client_message_list_destroy(client_message_list_t* client_message_list);

#define CLIENT_MESSAGE_LIST_BEST_ESTIMATION_FAILURE 0
#define CLIENT_MESSAGE_LIST_BEST_ESTIMATION_SUCCESS 1

int client_message_list_best_estimation(client_message_list_t* client_message_list, int file_descriptor, queue_element_t* queue_element);

#define CLIENT_MESSAGE_LIST_INSERT_FAILURE 0
#define CLIENT_MESSAGE_LIST_INSERT_SUCCESS 1

int client_message_list_insert(client_message_list_t* client_message_list, long int client_id, int file_descriptor);
