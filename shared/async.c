#include "async.h"

pid_t fork_and_execl(char *program_name, void *argument) {
    pid_t process_id = fork();

    if (process_id == -1) {
        perror("[async >> fork_and_execl] error forking");
        exit(EXIT_FAILURE);
    }

    if (process_id == 0) {
        execl(program_name, program_name, argument, NULL);

        perror("[async >> fork_and_execl] error execl");
        exit(EXIT_FAILURE);
    }

    return process_id;
}

pthread_t create_thread(void *routine(void *)) {
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, routine, NULL) == -1) {
        perror("[async >> create_thread] error creating thread");
        exit(EXIT_FAILURE);
    }

    return thread_id;
}

pthread_t create_thread_with_argument(void *routine(void *), void *argument) {
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, routine, argument) == -1) {
        perror("[async >> create_thread_with_argument] error creating thread");
        exit(EXIT_FAILURE);
    }

    return thread_id;
}

void initialize_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_init(mutex, NULL) != 0) {
        perror("[async >> initialize_mutex] error creating mutex");
        exit(EXIT_FAILURE);
    }
}

void lock_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_lock(mutex) != 0) {
        perror("[async >> initialize_mutex] error locking mutex");
        exit(EXIT_FAILURE);
    }
}

void unlock_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_unlock(mutex) != 0) {
        perror("[async >> unlock_mutex] error unlocking mutex");
        exit(EXIT_FAILURE);
    }
}

void destroy_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_destroy(mutex) != 0) {
        perror("[async >> destroy_mutex] error destroing mutex");
        exit(EXIT_FAILURE);
    }
}

void initialize_semaphore(sem_t *semaphore, int initial_value) {
    if (sem_init(semaphore, 1, initial_value) == -1) {
        perror("[async >> initialize_semaphore] error creating semaphore");
        exit(EXIT_FAILURE);
    }
}

void destroy_semaphore(sem_t* semaphore) {
    if (sem_destroy(semaphore) != 0) {
        perror("[async >> destroy_semaphore] error destroing semaphore");
        exit(EXIT_FAILURE);
    }
}

void sleep_milliseconds(long int milliseconds) {
    struct timespec ts;
    int res;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);
}