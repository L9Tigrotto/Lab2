#include "concurrent_message_list.h"
#include "estimation_list.h"

int client_message_list_initialize(client_message_list_t* client_message_list) {
    if (client_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> client_message_list_initialize] error null pointer.\n");
        return CLIENT_MESSAGE_LIST_INITIALIZE_FAILURE;
    }

    client_message_list->head = NULL;

    initialize_mutex(&client_message_list->mutex);

    return CLIENT_MESSAGE_LIST_INITIALIZE_SUCCESS;
}

int client_message_list_destroy(client_message_list_t* client_message_list) {

    if (client_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> client_message_list_destroy] error null pointer.\n");
        return CLIENT_MESSAGE_LIST_DESTROY_FAILURE;
    }

    client_message_node_t* next_client_message_node;
    for (client_message_node_t* current_client_message_node = client_message_list->head;
            current_client_message_node;
            current_client_message_node = next_client_message_node) {

        next_client_message_node = current_client_message_node->next;
        free(current_client_message_node);
    }

    destroy_mutex(&client_message_list->mutex);

    return CLIENT_MESSAGE_LIST_DESTROY_SUCCESS;
}

client_message_node_t* client_message_list_find_file_descriptor(client_message_list_t* client_message_list, int file_descriptor) {
    
    if (client_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> client_message_list_find_file_descriptor] error null pointer.\n");
        return NULL;
    }

    lock_mutex(&client_message_list->mutex);

    client_message_node_t* node = NULL;
    for (client_message_node_t* current_client_message_node = client_message_list->head;
            !node && current_client_message_node;
            current_client_message_node = current_client_message_node->next) {
        
        if (current_client_message_node->file_descriptor == file_descriptor) { node = current_client_message_node; }
    }

    unlock_mutex(&client_message_list->mutex);

    return node;
}

client_message_node_t* client_message_list_find_and_remove_file_descriptor(client_message_list_t* client_message_list, int file_descriptor) {

    if (client_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> client_message_list_find_file_descriptor] error null pointer.\n");
        return NULL;
    }

    lock_mutex(&client_message_list->mutex);

    client_message_node_t* node = NULL;
    if (client_message_list->head->file_descriptor == file_descriptor) {
        node = client_message_list->head;
        client_message_list->head = client_message_list->head->next;
    }
    else {
        client_message_node_t* current_client_message_node;
        for (current_client_message_node = client_message_list->head;
                current_client_message_node->next;
                current_client_message_node = current_client_message_node->next) {
            
            if (current_client_message_node->next->file_descriptor == file_descriptor) {
                node = current_client_message_node->next;
                current_client_message_node->next = current_client_message_node->next->next;
                break;
            }
        }
    }

    unlock_mutex(&client_message_list->mutex);

    return node;
}

int client_message_list_best_estimation(client_message_list_t* client_message_list, int file_descriptor, queue_element_t* queue_element) {

    client_message_node_t* client_message_node = client_message_list_find_and_remove_file_descriptor(client_message_list, file_descriptor);
    
    if (client_message_node == NULL) { return CLIENT_MESSAGE_LIST_BEST_ESTIMATION_FAILURE; }

    queue_element->client_id = client_message_node->client_id;
    queue_element->estimated_secret = client_message_node->secret;

    free(client_message_node);
    return CLIENT_MESSAGE_LIST_BEST_ESTIMATION_SUCCESS;
}


int client_message_list_insert(client_message_list_t* client_message_list, long int client_id, int file_descriptor) {
    
    if (client_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> client_message_list_insert] error null pointer.\n");
        return CLIENT_MESSAGE_LIST_INSERT_FAILURE;
    }

    client_message_node_t* client_estimation_node = client_message_list_find_file_descriptor(client_message_list, file_descriptor);

    if (client_estimation_node == NULL) {

        client_estimation_node = (client_message_node_t*)malloc(sizeof(client_message_node_t));
    
        if (client_estimation_node == NULL) {
            fprintf(stderr, "[concurrent_message_list >> client_message_list_insert] error alocating memory");
            return CLIENT_MESSAGE_LIST_INSERT_FAILURE;
        }

        client_estimation_node->file_descriptor = file_descriptor;
        client_estimation_node->client_id = client_id;
        clock_gettime(CLOCK_MONOTONIC, &client_estimation_node->last_time);
        client_estimation_node->secret = MAX_SECRET;

        lock_mutex(&client_message_list->mutex);

        client_estimation_node->next = client_message_list->head;
        client_message_list->head = client_estimation_node;

        unlock_mutex(&client_message_list->mutex);
    } else {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        double seconds = now.tv_sec - client_estimation_node->last_time.tv_sec;
        double nanoseconds = now.tv_nsec - client_estimation_node->last_time.tv_nsec;

        if (nanoseconds < 0) {
            nanoseconds += 1000000000;
            seconds--;
        }
        long int elapsed = (long int)(seconds * 1000 + nanoseconds / 1000000);

        lock_mutex(&client_message_list->mutex);

        client_estimation_node->last_time = now;
        if (client_estimation_node->secret > elapsed) { client_estimation_node->secret = elapsed; }
        
        unlock_mutex(&client_message_list->mutex);
    }

    return CLIENT_MESSAGE_LIST_INSERT_SUCCESS;
}
