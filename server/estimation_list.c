#include "estimation_list.h"

int estimation_list_initialize(estimation_list_t* estimation_list) {
    if (estimation_list == NULL) {
        fprintf(stderr, "[estimation_list >> estimation_list_initialize] error null pointer.\n");
        return ESTIMATION_LIST_INITIALIZE_FAILURE;
    }

    estimation_list->head = NULL;

    return ESTIMATION_LIST_INSERT_SUCCESS;
}

int estimation_list_destroy(estimation_list_t* estimation_list) {
    if (estimation_list == NULL) {
        fprintf(stderr, "[estimation_list >> estimation_list_destroy] error null pointer.\n");
        return ESTIMATION_LIST_DESTROY_FAILURE;
    }

    estimation_node_t* next_estimation_node;
    for (estimation_node_t* current_estimation_node = estimation_list->head;
            current_estimation_node;
            current_estimation_node = next_estimation_node) {
        
        next_estimation_node = current_estimation_node->next;
        free(current_estimation_node);
    }

    return ESTIMATION_LIST_DESTROY_SUCCESS;
}

int estimation_list_insert(estimation_list_t* estimation_list, int estimation, int times) {
    if (estimation_list == NULL) {
        fprintf(stderr, "[estimation_list >> estimation_list_insert] error null pointer.\n");
        return ESTIMATION_LIST_INSERT_FAILURE;
    }

    estimation_node_t* current_estimation_node;
    for (current_estimation_node = estimation_list->head;
            current_estimation_node;
            current_estimation_node = current_estimation_node->next) {
        
        if (abs(current_estimation_node->estimation - estimation) < MAX_ERROR) { break; }
    }

    if (current_estimation_node == NULL) {
        current_estimation_node = (estimation_node_t*)malloc(sizeof(estimation_node_t));

        current_estimation_node->estimation = estimation;
        current_estimation_node->times = times;

        current_estimation_node->next = estimation_list->head;
        estimation_list->head = current_estimation_node;
    } else { current_estimation_node->times += times; }


    return ESTIMATION_LIST_INSERT_SUCCESS;
}

void estimation_list_best(estimation_list_t* estimation_list, estimation_node_t* best_estimation) {
    if (estimation_list == NULL) {
        fprintf(stderr, "[estimation_list >> estimation_list_best] error null pointer.\n");
        return;
    }

    for (estimation_node_t* current_estimation_node = estimation_list->head;
            current_estimation_node;
            current_estimation_node = current_estimation_node->next) {
        
        if (best_estimation->times < current_estimation_node->times) { 
            best_estimation->estimation = current_estimation_node->estimation;
            best_estimation->times = current_estimation_node->times;
        }
        else if (best_estimation->times == current_estimation_node->times &&
                best_estimation->estimation > current_estimation_node->estimation) {
            best_estimation->estimation = current_estimation_node->estimation;
        }
    }
}