#include "numbers.h"

int convert_string_to_positive_number(char* string, int* number) {
    
    int length = strlen(string);
    int is_valid = 1;

    for (int i = 0; is_valid && i < length; i++) {
        is_valid = string[i] >= '0' && string[i] <= '9';
    }

    // "atoi return value: the converted value or 0 on error."
    // https://man7.org/linux/man-pages/man3/atoi.3.html
    
    *number = atoi(string);
    return is_valid;
}


void initialize_random() {
    static int is_random_initialized = 0;

    if (is_random_initialized) { return; }

    srand(time(NULL) * getpid());

    is_random_initialized = 1;
}

int generate_random_int(int min, int max) {
    initialize_random();

    int random_int = (rand() % (max - min + 1)) + min;

    return random_int;
}

long int generate_random_long() {
    initialize_random();

    long int part1 = (long int)rand() << 32;
    long int part2 = (long int)rand();

    long int random_long = part1 | part2;

    return random_long;
}

void generate_unique_int(int* array, int count, int min, int max) {
    int_list_t int_list;
    int_list_initialize(&int_list);

    for (int i = min; i <= max; i++) { int_list_insert(&int_list, i); }

    for (int i = 0; i < count; i++) {
        int random_index = generate_random_int(0, int_list.count - 1);
        int_list_remove_at(&int_list, random_index, &array[i]);
    }

    int_list_destroy(&int_list);
}