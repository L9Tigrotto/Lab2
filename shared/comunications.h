#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */

#include <wait.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

#define SHARED_MEMORY_NAME "/shm-progetto_lab2"

#define SERVER_IP "127.0.0.1"


void write_string_descriptor(int descriptor, char* string);
int read_descriptor(int descriptor, size_t size, void* content);
void close_descriptor(int descriptor);

int open_shared_memory(char* name, int flags);
void truncate_shared_memory(int shared_memory_descriptor, size_t size);
void* map_shared_memory(int shared_memory_descriptor, size_t size);

void unmap_shared_memory(void* pointer, size_t size);
void unlink_shared_memory(char* name);

void signal_process(pid_t process_id, int signal);
void wait_process(pid_t process_id);
void wait_thread(pthread_t thread_id);

sigset_t create_new_thread_signal_mask(int* signals, int count);
void set_new_thread_signal_mask(sigset_t* mask);
void wait_signal(sigset_t* mask);

int create_socket_descriptor();
void create_socket_address(struct sockaddr_in* socket_address, char* ip, int port);
void bind_and_listen_socket(int socket, struct sockaddr_in* address, size_t size);
int accept_client(int socket);
void connect_to(int socket_descriptor, struct sockaddr_in* socket_address, size_t size);

void write_long_to_socket(int socket, long int* value);

int select_read_with_timeout(fd_set* set, int max, int millisecond);