#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <signal.h>

#include <pthread.h>
#include <sys/wait.h>

#include <sys/time.h>
#include <errno.h>

#include <sys/select.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "concurrent_message_list.h"
#include "../shared/comunications.h"
#include "../shared/async.h"


#define MANAGER_COUNT 3
#define SELECT_CHECK_TIMEOUT_MILLS 5   

#define TEMP_TEXT_LENGTH 500

char temp_text[TEMP_TEXT_LENGTH];

int server_id;
int socket_descriptor;

concurrent_queue_t* concurrent_queue;

client_message_list_t client_message;

pthread_t accept_thread;
pthread_t manager_threads[MANAGER_COUNT];

pthread_mutex_t all_mutex;
fd_set all_descriptors;
int all_max;

fd_set* manager_sockets;
int* manager_maxs;
pthread_mutex_t* manager_mutexs;

int stop;


void validate_arguments(int argc, char* argv[], int* number);

int open_socket(char* ip, int port);

concurrent_queue_t* open_and_map_shared_memory(char* name);

void create_threads(pthread_t* accept, pthread_t* managers, int count);

void handle_signals();