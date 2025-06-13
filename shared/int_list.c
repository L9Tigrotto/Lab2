#include "int_list.h"

int int_list_initialize(int_list_t* int_list) {
    if (int_list == NULL) {
        fprintf(stderr, "[int_list >> int_list_initialize] error null pointer.\n");
        return INT_LIST_INITIALIZE_FAILURE;
    }

    int_list->head = NULL;
    int_list->count = 0;

    return INT_LIST_INITIALIZE_SUCCESS;
}

int int_list_destroy(int_list_t* int_list) {
    if (int_list == NULL) {
        fprintf(stderr, "[int_list >> int_list_destroy] error null pointer.\n");
        return INT_LIST_DESTROY_FAILURE;
    }

    int_node_t* next_node;

    for(int_node_t* current_node = int_list->head; current_node; current_node = next_node) {
        next_node = current_node->next;
        free(current_node);
    }

    return INT_LIST_DESTROY_SUCCESS;
}

int int_list_insert(int_list_t* int_list, int value) {
    if (int_list == NULL) {
        fprintf(stderr, "[int_list >> int_list_insert] error null pointer.\n");
        return INT_LIST_INSERT_FAILURE;
    }

    int_node_t* new_node = (int_node_t*)malloc(sizeof(int_node_t));
    new_node->next = int_list->head;
    new_node->value = value;
    
    int_list->head = new_node;
    int_list->count++;

    return INT_LIST_INSERT_SUCCESS;
}

int int_list_remove_at(int_list_t* int_list, int index, int* value) {
    if (int_list == NULL) {
        fprintf(stderr, "[int_list >> int_list_remove] error null pointer.\n");
        return INT_LIST_REMOVE_AT_FAILURE;
    }
    
    if (int_list->count == 0) { return INT_LIST_REMOVE_AT_FAILURE; }

    int_node_t* current_node = int_list->head;
    int_node_t* node_to_remove = NULL;
    for (int i = 0; i < index - 1; i++) {  current_node = current_node->next; }

    if (index == 0) {
        node_to_remove = int_list->head;
        int_list->head = int_list->head->next;
    }
    else {
        node_to_remove = current_node->next;
        current_node->next = current_node->next->next;
    }

    *value = node_to_remove->value;
    free(node_to_remove);
    int_list->count--;

    return INT_LIST_REMOVE_AT_SUCCESS;
}

