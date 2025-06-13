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

#include <time.h>

#include "../shared/comunications.h"
#include "concurrent_message_list.h"

#define SERVER_ID_MAX_DECIMALS 10

#define TEMP_TEXT_LENGTH 500

char temp_text[TEMP_TEXT_LENGTH];

int number_of_servers;
pid_t* server_processes;

server_message_list_t server_messages;
concurrent_queue_t* concurrent_queue;

pthread_t read_thread;

void validate_arguments(int argc, char* argv[], int* number_of_servers);

concurrent_queue_t* open_exclusive_and_map_shared_memory(char* name);

void fork_and_exec_servers(pid_t* array, int length);

void create_read_thread(pthread_t* thread);

void handle_signals();