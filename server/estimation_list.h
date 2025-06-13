#include <stdio.h>
#include <stdlib.h>

#define MAX_ERROR 25

typedef struct estimation_node_t {
    int estimation;
    int times;

    struct estimation_node_t* next;
} estimation_node_t;

typedef struct estimation_list_t {
    estimation_node_t* head;
} estimation_list_t;

#define ESTIMATION_LIST_INITIALIZE_FAILURE 0
#define ESTIMATION_LIST_INITIALIZE_SUCCESS 1

int estimation_list_initialize(estimation_list_t* estimation_list);

#define ESTIMATION_LIST_DESTROY_FAILURE 0
#define ESTIMATION_LIST_DESTROY_SUCCESS 1

int estimation_list_destroy(estimation_list_t* estimation_list);

#define ESTIMATION_LIST_INSERT_FAILURE 0
#define ESTIMATION_LIST_INSERT_SUCCESS 1

int estimation_list_insert(estimation_list_t* estimation_list, int estimation, int times);

void estimation_list_best(estimation_list_t* estimation_list, estimation_node_t* best_estimation);

