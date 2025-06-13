#include "utilities.h"
#include "../shared/numbers.h"

typedef struct manager_argument_t {
    fd_set* my_set;
    int* my_max;
    pthread_mutex_t* my_mutex;
} manager_argument_t;

void validate_arguments(int argc, char* argv[], int* number) {
    // se gli argomenti sono 2 allora controllo che i sia un numero > 0;

    if (argc != 2) {
        fprintf(stdout, "usage: %s i.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!convert_string_to_positive_number(argv[1], number) || *number == 0) {
        fprintf(stdout, "i must be a number greater than 0.\n");
        exit(EXIT_FAILURE);
    }

    stop = 0;

    snprintf(temp_text, TEMP_TEXT_LENGTH, "SERVER %d ACTIVE.\n", server_id);
    write_string_descriptor(1, temp_text);
}

int open_socket(char* ip, int port) {
    struct sockaddr_in sock_in;
    create_socket_address(&sock_in, ip, port);
    int sd = create_socket_descriptor();
    bind_and_listen_socket(sd, &sock_in, sizeof(sock_in));

    client_message_list_initialize(&client_message);

    return sd;
}

concurrent_queue_t* open_and_map_shared_memory(char* name) {
    int shared_memory_descriptor = open_shared_memory(name, O_RDWR);
    size_t size = sizeof(concurrent_queue_t);
    concurrent_queue_t* queue = (concurrent_queue_t*)map_shared_memory(shared_memory_descriptor, size);

    return queue;
}


void* accept_thread_routine(void* argument) {
    sigset_t mask = create_new_thread_signal_mask(NULL, 0);
    set_new_thread_signal_mask(&mask);

    fd_set my_set;
    FD_ZERO(&my_set);
    FD_SET(socket_descriptor, &my_set);

    int result;
    int index = 0;

    char my_temp_text[TEMP_TEXT_LENGTH];

    while (1) {
        fd_set read_set = my_set;

        result = select_read_with_timeout(&read_set, socket_descriptor + 1, SELECT_CHECK_TIMEOUT_MILLS);
        
        if (stop) { break; }
        if (result == 0) { continue; }

        int client_socket = accept_client(socket_descriptor);

        snprintf(my_temp_text, TEMP_TEXT_LENGTH, "SERVER %d CONNECTED FROM CLIENT.\n", server_id);
        write_string_descriptor(1, my_temp_text);

        lock_mutex(&all_mutex);
        if (all_max < client_socket) { all_max = client_socket; }
        FD_SET(client_socket, &all_descriptors);
        unlock_mutex(&all_mutex);

        lock_mutex(&manager_mutexs[index]);
        if (manager_maxs[index] < client_socket) { manager_maxs[index] = client_socket; }
        FD_SET(client_socket, &manager_sockets[index]);
        unlock_mutex(&manager_mutexs[index]);

        index = (index + 1) % MANAGER_COUNT;
    }

    for (int i = 0; i < MANAGER_COUNT; i++) { wait_thread(manager_threads[i]); }

    free(manager_sockets);
    free(manager_mutexs);
    free(manager_maxs);
    destroy_mutex(&all_mutex);

    while (all_max >= 0) {
        if (FD_ISSET(all_max, &all_descriptors)) { close_descriptor(all_max); }
        all_max--;
    }

    return NULL;
}

void* manager_thread_routine(void* argument) {
    sigset_t mask = create_new_thread_signal_mask(NULL, 0);
    set_new_thread_signal_mask(&mask);

    manager_argument_t* my_argument = (manager_argument_t*)argument;

    char my_temp_text[TEMP_TEXT_LENGTH];

    while (1) {
        lock_mutex(my_argument->my_mutex);
        fd_set read_set = *my_argument->my_set;
        int max = *my_argument->my_max;
        unlock_mutex(my_argument->my_mutex);

        select_read_with_timeout(&read_set, max + 1, SELECT_CHECK_TIMEOUT_MILLS);

        if (stop) { break; }
        
        for (int selected_socket = 0; selected_socket < max + 1; selected_socket++) {
            if (!FD_ISSET(selected_socket, &read_set)) { continue; }
            struct timespec recive_time;
            clock_gettime(CLOCK_MONOTONIC, &recive_time);
            long int client_id = 0;
            int readed_bytes = read_descriptor(selected_socket, sizeof(long int), &client_id);
            long int converted_id = be64toh(client_id);

            if (readed_bytes == 0) {
                lock_mutex(&all_mutex);
                FD_CLR(selected_socket, &all_descriptors);
                while(!FD_ISSET(all_max, &all_descriptors) && all_max >= 0) { all_max--; }
                unlock_mutex(&all_mutex);

                lock_mutex(my_argument->my_mutex);
                FD_CLR(selected_socket, my_argument->my_set);
                while(!FD_ISSET(*my_argument->my_max, my_argument->my_set) && *my_argument->my_max >= 0) { (*my_argument->my_max)--; }
                unlock_mutex(my_argument->my_mutex);

                queue_element_t queue_element;
                queue_element.client_id = 0;
                queue_element.estimated_secret = MAX_SECRET;
                queue_element.server_id = server_id;

                int result = client_message_list_best_estimation(&client_message, selected_socket, &queue_element);

                if (result != CLIENT_MESSAGE_LIST_BEST_ESTIMATION_FAILURE) {
                    concurrent_queue_enqueue(concurrent_queue, queue_element);
                }

                snprintf(my_temp_text, TEMP_TEXT_LENGTH, "SERVER %d CLOSING %ld ESTIMATE %d.\n", server_id, queue_element.client_id, queue_element.estimated_secret);
                write_string_descriptor(1, my_temp_text);
                
                close_descriptor(selected_socket);
            } else {
                snprintf(my_temp_text, TEMP_TEXT_LENGTH, "SERVER %d INCOMING FROM %ld @ %lld.%.9ld.\n", server_id, converted_id, (long long)recive_time.tv_sec, recive_time.tv_nsec);
                write_string_descriptor(1, my_temp_text);

                client_message_list_insert(&client_message, converted_id, selected_socket);
            }
        }
    }

    destroy_mutex(my_argument->my_mutex);
    free(my_argument);

    return NULL;
}

void create_threads(pthread_t* accept, pthread_t* managers, int count) {
    manager_sockets = (fd_set*)malloc(sizeof(fd_set) * count);
    manager_maxs = (int*)malloc(sizeof(int) * count);
    manager_mutexs = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * count);

    initialize_mutex(&all_mutex);
    FD_ZERO(&all_descriptors);
    all_max = -1;

    for (int i = 0; i < count; i++) {
        manager_argument_t* manager_argument = (manager_argument_t*)malloc(sizeof(manager_argument_t));
        manager_argument->my_mutex = &manager_mutexs[i];
        initialize_mutex(&manager_mutexs[i]);

        manager_argument->my_set = &manager_sockets[i];
        FD_ZERO(&manager_sockets[i]);

        manager_argument->my_max = &manager_maxs[i];
        manager_maxs[i] = -1;

        managers[i] = create_thread_with_argument(manager_thread_routine, manager_argument);
    }

    *accept = create_thread(accept_thread_routine);
}


void handle_signals() {
    int signal = SIGINT;
    sigset_t mask = create_new_thread_signal_mask(&signal, 1);
    set_new_thread_signal_mask(&mask);

    wait_signal(&mask);

    stop = 1;

    wait_thread(accept_thread);

    close_descriptor(socket_descriptor);

    snprintf(temp_text, TEMP_TEXT_LENGTH, "SERVER %d EXITING.\n", server_id);
    write_string_descriptor(1, temp_text);

    unmap_shared_memory(concurrent_queue, sizeof(concurrent_queue_t));

    exit(EXIT_SUCCESS);
}




