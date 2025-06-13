#include "concurrent_message_list.h"

int server_message_list_initialize(server_message_list_t* server_message_list) {
    if (server_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> server_message_list_initialize] error null pointer.\n");
        return SERVER_MESSAGE_LIST_INITIALIZE_FAILURE;
    }

    server_message_list->head = NULL;

    initialize_mutex(&server_message_list->mutex);

    return SERVER_MESSAGE_LIST_INITIALIZE_SUCCESS;
}

int server_message_list_destroy(server_message_list_t* server_message_list) {

    if (server_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> server_message_list_destroy] error null pointer.\n");
        return SERVER_MESSAGE_LIST_DESTROY_FAILURE;
    }

    server_message_node_t* next_server_message_node;
    for (server_message_node_t* current_server_message_node = server_message_list->head;
            current_server_message_node;
            current_server_message_node = next_server_message_node) {

        next_server_message_node = current_server_message_node->next;
        free(current_server_message_node);
    }

    destroy_mutex(&server_message_list->mutex);

    return SERVER_MESSAGE_LIST_DESTROY_SUCCESS;
}

server_message_node_t* server_message_list_find_client_id(server_message_list_t* server_message_list, long int client_id) {

    if (server_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> server_message_list_find_file_descriptor] error null pointer.\n");
        return NULL;
    }

    lock_mutex(&server_message_list->mutex);

    server_message_node_t* node = NULL;
    for (server_message_node_t* current_server_message_node = server_message_list->head;
            current_server_message_node;
            current_server_message_node = current_server_message_node->next) {
        if (current_server_message_node->client_id == client_id) {
            node = current_server_message_node;
            break;
        }
    }

    unlock_mutex(&server_message_list->mutex);

    return node;
}


int server_message_list_insert(server_message_list_t* server_message_list, long int client_id, int secret) {
    
    if (server_message_list == NULL) {
        fprintf(stderr, "[concurrent_message_list >> server_message_list_insert] error null pointer.\n");
        return SERVER_MESSAGE_LIST_INSERT_FAILURE;
    }

    server_message_node_t* server_estimation_node = server_message_list_find_client_id(server_message_list, client_id);

    if (server_estimation_node == NULL) {

        server_estimation_node = (server_message_node_t*)malloc(sizeof(server_message_node_t));
    
        if (server_estimation_node == NULL) {
            fprintf(stderr, "[concurrent_message_list >> server_message_list_insert] error alocating memory");
            return SERVER_MESSAGE_LIST_INSERT_FAILURE;
        }

        server_estimation_node->client_id = client_id;
        server_estimation_node->secret = secret;
        server_estimation_node->number_of_servers = 1;

        lock_mutex(&server_message_list->mutex);

        server_estimation_node->next = server_message_list->head;
        server_message_list->head = server_estimation_node;

        unlock_mutex(&server_message_list->mutex);
    } else {
        lock_mutex(&server_message_list->mutex);

        if (server_estimation_node->secret > secret) { server_estimation_node->secret = secret; }
        server_estimation_node->number_of_servers++;

        unlock_mutex(&server_message_list->mutex);
    }

    return SERVER_MESSAGE_LIST_INSERT_SUCCESS;
}
