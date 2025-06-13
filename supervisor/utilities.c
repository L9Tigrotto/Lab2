#include "utilities.h"
#include "../shared/numbers.h"
#include "../shared/comunications.h"
#include "../shared/async.h"

void validate_arguments(int argc, char* argv[], int* number_of_servers) {

    // se gli argomenti sono 2 allora controllo che S sia un numero > 0;

    if (argc != 2) {
        fprintf(stdout, "usage: %s S.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!convert_string_to_positive_number(argv[1], number_of_servers) || *number_of_servers == 0) {
        fprintf(stdout, "S must be a number greater than 0.\n");
        exit(EXIT_FAILURE);
    }

    snprintf(temp_text, TEMP_TEXT_LENGTH, "SUPERVISOR STARTING %d SERVER.\n", *number_of_servers);
    write_string_descriptor(1, temp_text);
}

concurrent_queue_t* open_exclusive_and_map_shared_memory(char* name) {

    int shared_memory_descriptor = open_shared_memory(name, O_CREAT | O_EXCL | O_RDWR);
    size_t size = sizeof(concurrent_queue_t);
    truncate_shared_memory(shared_memory_descriptor, size);
    concurrent_queue_t* queue = (concurrent_queue_t*)map_shared_memory(shared_memory_descriptor, size);

    if (concurrent_queue_initialize(queue) == CONCURRENT_QUEUE_INITIALIZE_FAILURE) { exit(EXIT_FAILURE); }
    if (server_message_list_initialize(&server_messages) == SERVER_MESSAGE_LIST_INITIALIZE_FAILURE) { exit(EXIT_FAILURE); }
    return queue;
}


void print_table(int descriptor) {
    lock_mutex(&server_messages.mutex);

    for (server_message_node_t* current_node = server_messages.head;
            current_node; current_node = current_node->next) {
        snprintf(temp_text, TEMP_TEXT_LENGTH, "SUPERVISOR ESTIMATE %d FOR %ld BASED ON %d.\n", current_node->secret, current_node->client_id, current_node->number_of_servers);
        write_string_descriptor(descriptor, temp_text);
    }

    unlock_mutex(&server_messages.mutex);
}

void handle_signals() {
    int signal = SIGINT;
    sigset_t mask = create_new_thread_signal_mask(&signal, 1);
    set_new_thread_signal_mask(&mask);

    struct timespec last_recived;
    last_recived.tv_nsec = 0;
    last_recived.tv_sec = 0;
    //clock_gettime(CLOCK_MONOTONIC, &now);
    //time_t last_recived = 0;

    while (1)
    {
        wait_signal(&mask);

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        double seconds = now.tv_sec - last_recived.tv_sec;
        double nanoseconds = now.tv_nsec - last_recived.tv_nsec;

        if (nanoseconds < 0) {
            nanoseconds += 1000000000;
            seconds--;
        }
        long int elapsed = (long int)(seconds + nanoseconds / 1000000000);

        print_table(2);

        if(elapsed <= 1) {
            print_table(1);
            write_string_descriptor(1, "SUPERVISOR EXITING.\n");
            concurrent_queue_disable_request(concurrent_queue);

            for (int i = 0; i < number_of_servers; i++) { signal_process(server_processes[i], SIGINT); }
            for (int i = 0; i < number_of_servers; i++) { wait_process(server_processes[i]); }
            wait_thread(read_thread);

            concurrent_queue_destroy(concurrent_queue);
            server_message_list_destroy(&server_messages);
            unmap_shared_memory(concurrent_queue, sizeof(concurrent_queue_t));
            unlink_shared_memory(SHARED_MEMORY_NAME);
            free(server_processes);

            exit(EXIT_SUCCESS);
        }

        last_recived = now;
    }
}



void fork_and_exec_servers(pid_t* array, int length) {
    char server_id_string[SERVER_ID_MAX_DECIMALS];

    for (int i = 0; i < length; i++) {
        snprintf(server_id_string, sizeof(char) * SERVER_ID_MAX_DECIMALS, "%d", (i + 1));
        array[i] = fork_and_execl("executable/server", server_id_string);
    }
}


void* read_thread_routine(void* unused) {
    sigset_t mask = create_new_thread_signal_mask(NULL, 0);
    set_new_thread_signal_mask(&mask);

    char my_temp[TEMP_TEXT_LENGTH];

    while (1) {
        queue_element_t new_element;

        int result = concurrent_queue_dequeue(concurrent_queue, &new_element);

        if (result == CONCURRENT_QUEUE_DEQUEUE_DISABLE) { break; }

        server_message_list_insert(&server_messages, new_element.client_id, new_element.estimated_secret);

        snprintf(my_temp, TEMP_TEXT_LENGTH, "SUPERVISOR ESTIMATE %d FOR %ld FROM %d.\n", new_element.estimated_secret, new_element.client_id, new_element.server_id);
        write_string_descriptor(1, my_temp);
    }

    return NULL;
}

void create_read_thread(pthread_t* thread) { *thread = create_thread(read_thread_routine); }






