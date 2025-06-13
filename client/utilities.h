#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */

#include "../shared/numbers.h"
#include "../shared/comunications.h"
#include "../shared/async.h"

#define CLIENT_MIN_SECRET 1
#define CLIENT_MAX_SECRET 3000

#define TEMP_TEXT_LENGTH 500

char temp_text[TEMP_TEXT_LENGTH];


void validate_arguments(int argc, char* argv[], int* number_of_servers, int* number_of_connections, int* number_of_messages);

void generate(int* secret, long int* id);

int* connect_to_random_servers(int number_of_servers, int number_of_connections);

void send_messages(long int my_id, int secret, int* sockets, int count, int number_of_messages);

void close_connections(int* descriptors, int number_of_connections, long int my_id);