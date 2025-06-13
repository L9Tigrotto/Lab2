
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>

#include <errno.h>
#include <semaphore.h>

pid_t fork_and_execl(char* program_name, void* argument);

pthread_t create_thread(void* routine(void*));
pthread_t create_thread_with_argument(void* routine(void*), void* argument);

void initialize_mutex(pthread_mutex_t* mutex);
void lock_mutex(pthread_mutex_t* mutex);
void unlock_mutex(pthread_mutex_t* mutex);
void destroy_mutex(pthread_mutex_t* mutex);

void initialize_semaphore(sem_t* semaphore, int initial_value);
void destroy_semaphore(sem_t* semaphore);


void sleep_milliseconds(long int milliseconds);