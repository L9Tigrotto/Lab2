#include <stdio.h>
#include <stdlib.h>

typedef struct int_node_t {
    int value;

    struct int_node_t* next;
} int_node_t;

typedef struct int_list_t {
    int_node_t* head;
    int count;
} int_list_t;

#define INT_LIST_INITIALIZE_FAILURE 0
#define INT_LIST_INITIALIZE_SUCCESS 1

int int_list_initialize(int_list_t* int_list);

#define INT_LIST_DESTROY_FAILURE 0
#define INT_LIST_DESTROY_SUCCESS 1

int int_list_destroy(int_list_t* int_list);

#define INT_LIST_INSERT_FAILURE 0
#define INT_LIST_INSERT_SUCCESS 1

int int_list_insert(int_list_t* int_list, int value);

#define INT_LIST_REMOVE_AT_FAILURE 0
#define INT_LIST_REMOVE_AT_SUCCESS 1

int int_list_remove_at(int_list_t* int_list, int index, int* value);

