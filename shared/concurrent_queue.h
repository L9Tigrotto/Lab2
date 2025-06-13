#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include <errno.h>

#define CONCURRENT_QUEUE_INITIALIZE_FAILURE 0
#define CONCURRENT_QUEUE_INITIALIZE_SUCCESS 1

#define CONCURRENT_QUEUE_DESTROY_FAILURE 0
#define CONCURRENT_QUEUE_DESTROY_SUCCESS 1

#define CONCURRENT_QUEUE_ENQUEUE_FAILURE 0
#define CONCURRENT_QUEUE_ENQUEUE_SUCCESS 1
#define CONCURRENT_QUEUE_ENQUEUE_DISABLE 2

#define CONCURRENT_QUEUE_DEQUEUE_FAILURE 0
#define CONCURRENT_QUEUE_DEQUEUE_SUCCESS 1
#define CONCURRENT_QUEUE_DEQUEUE_DISABLE 2

#define CONCURRENT_QUEUE_DISABLE_FAILURE 0
#define CONCURRENT_QUEUE_DISABLE_SUCCESS 1

#define CONCURRENT_BUFFER_CAPACITY 500
#define CONCURRENT_SEMAPHORE_TIMEOUT_MILLIS 5


// definisco l'elemento di cui sarà composta la coda concorrente
typedef struct queue_element_t {
    long int client_id;
    int estimated_secret;
    int server_id;
} queue_element_t;


// definisco la coda concorrente con i meccanismi per la concorrenza
typedef struct concurrent_queue_t
{
    // l'indice dove scrivere il prossimo elemento.
    int write_index;

    // l'indice dove leggere il prossimo elemento.
    int read_index;

    // l'array circolare che contiene elementi di tipo queue_element_t.
    queue_element_t array[CONCURRENT_BUFFER_CAPACITY];

    // la mutex per garantire la mutua esclusione in lettura e scrittura.
    pthread_mutex_t mutex;

    // semaforo gestire gli sccrittori.
    sem_t write_semaphore;

    // semaforo gestire i lettori.
    sem_t read_semaphore;

    int request_enabled;
} concurrent_queue_t;

// inizializza con i valori di default la struttura concurrent_queue_t.
// in caso di errore verrà stampato un messaggio su stderr e verrà restituito
// CONCURRENT_QUEUE_INITIALIZE_FAILURE, altrimenti CONCURRENT_QUEUE_INITIALIZE_SUCCESS.
int concurrent_queue_initialize(concurrent_queue_t* concurrent_queue);

// distrugge la struttura concurrent_queue_t.
// in caso di errore verrà stampato un messaggio su stderr e verrà restituito
// CONCURRENT_QUEUE_DESTROY_FAILURE, altrimenti CONCURRENT_QUEUE_DESTROY_SUCCESS.
int concurrent_queue_destroy(concurrent_queue_t* concurrent_queue);

// accoda un elemento alla struttura concurrent_queue_t.
// la chiamata è bloccante in caso il buffer sia utilizzato da qualcun altro
// o nel caso sia pieno e non uscirà dalla funzione finche l'elemento non sarà inserito.
// in caso di errore verrà ritornato CONCURRENT_QUEUE_ENQUEUE_FAILURE.
// in caso di successo verrà restituito CONCURRENT_QUEUE_ENQUEUE_SUCCESS.
int concurrent_queue_enqueue(concurrent_queue_t* concurrent_queue, queue_element_t element);

// de-accoda un elemento dalla struttura concurrent_queue_t.
// la chiamata è bloccante in caso il buffer sia utilizzato da qualcun altro
// o nel caso sia vuoto e non uscirà dalla funzione finche l'elemento non sarà letto.
// in caso di errore verrà ritornato CONCURRENT_QUEUE_DEQUEUE_FAILURE.
// in caso di successo verrà restituito CONCURRENT_QUEUE_DEQUEUE_SUCCESS.
int concurrent_queue_dequeue(concurrent_queue_t* concurrent_queue, queue_element_t* element);

int concurrent_queue_disable_request(concurrent_queue_t* concurrent_queue);