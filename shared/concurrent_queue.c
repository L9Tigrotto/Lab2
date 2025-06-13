#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "concurrent_queue.h"

int concurrent_queue_initialize(concurrent_queue_t* concurrent_queue) {
    // 1) controllo che concurrent_queue non sia nulla.
    // 2) imposto i valori di base per i vari campi.
    // 3) creo il mutex.
    // 4) creo il semaforo.

    if (concurrent_queue == NULL) {
        fprintf(stderr, "[concurrent_queue >> concurrent_queue_initialize] error NULL pointer.\n");
        return CONCURRENT_QUEUE_INITIALIZE_FAILURE;
    }

    concurrent_queue->write_index = 0;
    concurrent_queue->read_index = 0;

    concurrent_queue->request_enabled = 1;

    // 3)
    // pthread_mutex_init attr argument: "If attr is NULL, the default mutex attributes are used". 
    // pthread_mutex_init return value: "If successful, the pthread_mutex_destroy() 
    // and pthread_mutex_init() functions shall return zero; otherwise, an error number
    // shall be returned to indicate the error".
    // https://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html

    if (pthread_mutex_init(&concurrent_queue->mutex, NULL) != 0) {
        perror("[concurrent_queue >> concurrent_queue_initialize] error creating mutex");
        return CONCURRENT_QUEUE_INITIALIZE_FAILURE;
    }

    // 4)
    // return value: "sem_init() returns 0 on success; on error, -1 is returned, and
    // errno is set to indicate the error".
    // https://man7.org/linux/man-pages/man3/sem_init.3.html

    if (sem_init(&concurrent_queue->write_semaphore, 1, CONCURRENT_BUFFER_CAPACITY) == -1) {
        perror("[concurrent_queue >> concurrent_queue_initialize] error creating write semaphore");
        return CONCURRENT_QUEUE_INITIALIZE_FAILURE;
    }

    if (sem_init(&concurrent_queue->read_semaphore, 1, 0) == -1) {
        perror("[concurrent_queue >> concurrent_queue_initialize] error creating read semaphore");
        return CONCURRENT_QUEUE_INITIALIZE_FAILURE;
    }

    return CONCURRENT_QUEUE_INITIALIZE_SUCCESS;
}

// distrugge la struttura concurrent_queue_t.
// in caso di errore verrà stampato un messaggio su stderr e verrà restituito
// CONCURRENT_QUEUE_DESTROY_FAILURE, altrimenti CONCURRENT_QUEUE_DESTROY_SUCCESS.
int concurrent_queue_destroy(concurrent_queue_t* concurrent_queue) {

    if (concurrent_queue == NULL) {
        fprintf(stderr, "[concurrent_queue >> concurrent_queue_destroy] error NULL pointer.\n");
        return CONCURRENT_QUEUE_DESTROY_FAILURE;
    }

    int return_value = CONCURRENT_QUEUE_DESTROY_SUCCESS;

    if (pthread_mutex_destroy(&concurrent_queue->mutex) != 0) {
        perror("[concurrent_queue >> concurrent_queue_destroy] error destroing mutex");
        return_value = CONCURRENT_QUEUE_DESTROY_FAILURE;
    }

    // return value: "sem_destroy() returns 0 on success; on error, -1 is returned, and
    // errno is set to indicate the error".
    // https://man7.org/linux/man-pages/man3/sem_destroy.3.html

    if (sem_destroy(&concurrent_queue->write_semaphore) != 0) {
        perror("[concurrent_queue >> concurrent_queue_destroy] error destroing write semaphore");
        return_value = CONCURRENT_QUEUE_DESTROY_FAILURE;
    }

    if (sem_destroy(&concurrent_queue->read_semaphore) != 0) {
        perror("[concurrent_queue >> concurrent_queue_destroy] error destroing read semaphore");
        return_value = CONCURRENT_QUEUE_DESTROY_FAILURE;
    }

    return return_value;
}

int concurrent_queue_enqueue(concurrent_queue_t* concurrent_queue, queue_element_t element) {
    
    // 1) controllo che concurrent_queue non sia NULL.
    // 2) aspetto che il semaforo in scrittura sia disponibile.
    // 3) faccio la lock sulla mutex.
    // 4) inserisco l'elemento.
    // 5) faccio la unlock sulla mutex.
    // 6) sengalo il semaforo in lettura.

    // 1)
    if (concurrent_queue == NULL) {
        fprintf(stderr, "[concurrent_queue >> concurrent_queue_enqueue] error NULL pointer.\n");
        return CONCURRENT_QUEUE_ENQUEUE_FAILURE;
    }

    // 2)
    int captured = 0;
    struct timespec ts;
    while(!captured) {

        if(!concurrent_queue->request_enabled) { return CONCURRENT_QUEUE_ENQUEUE_DISABLE; }

        // return value: "clock_gettime(), clock_settime(), and clock_getres() return 0 for
        // success, or -1 for failure (in which case errno is set appropriately)".
        // https://man7.org/linux/man-pages/man3/clock_gettime.3.html

        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            perror("[concurrent_queue >> concurrent_queue_enqueue] error getting time");
            return CONCURRENT_QUEUE_ENQUEUE_FAILURE;
        }

        ts.tv_nsec += CONCURRENT_SEMAPHORE_TIMEOUT_MILLIS * 1000000;
        ts.tv_sec += ts.tv_nsec / 1000000000;
        ts.tv_nsec %= 1000000000;

        // return value: "All of these functions return 0 on success; on error, the value
        // of the semaphore is left unchanged, -1 is returned, and errno is
        // set to indicate the error".
        // https://man7.org/linux/man-pages/man3/sem_wait.3.html

        int result = sem_timedwait(&concurrent_queue->write_semaphore, &ts);
        if (result == -1) {
            if(errno == ETIMEDOUT) { continue; }
            perror("[concurrent_queue >> concurrent_queue_enqueue] error waiting write semaphore");
            return CONCURRENT_QUEUE_ENQUEUE_FAILURE;
        }

        else { captured = 1; }
    }

    // 3)
    // pthread_mutex_lock & pthread_mutex_unlock return value:
    // "If successful, the pthread_mutex_lock(), pthread_mutex_trylock(),
    // and pthread_mutex_unlock() functions shall return zero;
    // otherwise, an error number shall be returned to indicate the
    // error".
    // https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html

    if (pthread_mutex_lock(&concurrent_queue->mutex) != 0) {
        perror("[concurrent_queue >> concurrent_queue_enqueue] error lock mutex");
        return CONCURRENT_QUEUE_ENQUEUE_FAILURE;
    }

    // 4)
    concurrent_queue->array[concurrent_queue->write_index] = element;
    concurrent_queue->write_index = (concurrent_queue->write_index + 1) % CONCURRENT_BUFFER_CAPACITY;

    // 5)
    if (pthread_mutex_unlock(&concurrent_queue->mutex) != 0) {
        perror("[concurrent_queue >> concurrent_queue_enqueue] error unlock mutex");
        return CONCURRENT_QUEUE_ENQUEUE_FAILURE;
    }

    // 6)
    // return value: "sem_post() returns 0 on success; on error, the value of the
    // semaphore is left unchanged, -1 is returned, and errno is set to
    // indicate the error".
    // https://man7.org/linux/man-pages/man3/sem_post.3.html

    if (sem_post(&concurrent_queue->read_semaphore) == -1) {
        perror("[concurrent_queue >> concurrent_queue_enqueue] error posting read semaphore");
        return CONCURRENT_QUEUE_ENQUEUE_FAILURE;
    }
    
    return CONCURRENT_QUEUE_ENQUEUE_SUCCESS;
}

int concurrent_queue_dequeue(concurrent_queue_t* concurrent_queue, queue_element_t* element) {
    
    // 1) controllo che concurrent_queue non sia NULL.
    // 2) aspetto che il semaforo in lettura sia disponibile.
    // 3) faccio la lock sul mutex.
    // 4) copio l'elemento.
    // 6) faccio la unlock sulla mutex.
    // 6) segnalo il semaforo in scrittura.

    // 1)
    if (concurrent_queue == NULL) {
        fprintf(stderr, "[concurrent_queue >> concurrent_queue_dequeue] error NULL pointer.\n");
        return CONCURRENT_QUEUE_DEQUEUE_FAILURE;
    }

    // 2)
    int captured = 0;
    struct timespec ts;
    while(!captured) {

        if(!concurrent_queue->request_enabled) { return CONCURRENT_QUEUE_ENQUEUE_DISABLE; }

        // return value: "clock_gettime(), clock_settime(), and clock_getres() return 0 for
        // success, or -1 for failure (in which case errno is set appropriately)".
        // https://man7.org/linux/man-pages/man3/clock_gettime.3.html

        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            perror("[concurrent_queue >> concurrent_queue_dequeue] error getting time");
            return CONCURRENT_QUEUE_DEQUEUE_FAILURE;
        }

        ts.tv_nsec += CONCURRENT_SEMAPHORE_TIMEOUT_MILLIS * 1000000;
        ts.tv_sec += ts.tv_nsec / 1000000000;
        ts.tv_nsec %= 1000000000;

        // return value: "All of these functions return 0 on success; on error, the value
        // of the semaphore is left unchanged, -1 is returned, and errno is
        // set to indicate the error".
        // https://man7.org/linux/man-pages/man3/sem_wait.3.html

        int result = sem_timedwait(&concurrent_queue->read_semaphore, &ts);
        if (result == -1) {
            if(errno == ETIMEDOUT) { continue; }
            perror("[concurrent_queue >> concurrent_queue_dequeue] error waiting read semaphore");
            return CONCURRENT_QUEUE_DEQUEUE_FAILURE;
        }

        else { captured = 1; }
    }

    // 3)
    // pthread_mutex_lock & pthread_mutex_unlock return value:
    // "If successful, the pthread_mutex_lock(), pthread_mutex_trylock(),
    // and pthread_mutex_unlock() functions shall return zero;
    // otherwise, an error number shall be returned to indicate the
    // error".
    // https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html

    if (pthread_mutex_lock(&concurrent_queue->mutex) != 0) {
        perror("[concurrent_queue >> concurrent_queue_dequeue] error lock mutex");
        return CONCURRENT_QUEUE_DEQUEUE_FAILURE;
    }

    // 4)
    memcpy(element, &concurrent_queue->array[concurrent_queue->read_index], sizeof(queue_element_t));
    concurrent_queue->read_index = (concurrent_queue->read_index + 1) % CONCURRENT_BUFFER_CAPACITY;

    // 5)
    if (pthread_mutex_unlock(&concurrent_queue->mutex) != 0) {
        perror("[concurrent_queue >> concurrent_queue_dequeue] error unlock mutex");
        return CONCURRENT_QUEUE_DEQUEUE_FAILURE;
    }

    // 6)
    if (sem_post(&concurrent_queue->write_semaphore) == -1) {
        perror("[concurrent_queue >> concurrent_queue_enqueue] error posting write semaphore");
        return CONCURRENT_QUEUE_DEQUEUE_FAILURE;
    }
    
    return CONCURRENT_QUEUE_DEQUEUE_SUCCESS;
}

int concurrent_queue_disable_request(concurrent_queue_t* concurrent_queue) {
    if (pthread_mutex_lock(&concurrent_queue->mutex) != 0) {
        perror("[concurrent_queue >> concurrent_queue_disable_request] error lock mutex");
        return CONCURRENT_QUEUE_DISABLE_FAILURE;
    }

    concurrent_queue->request_enabled = 0;

    if (pthread_mutex_unlock(&concurrent_queue->mutex) != 0) {
        perror("[concurrent_queue >> concurrent_queue_disable_request] error unlock mutex");
        return CONCURRENT_QUEUE_DISABLE_FAILURE;
    }

    return CONCURRENT_QUEUE_DISABLE_SUCCESS;
}