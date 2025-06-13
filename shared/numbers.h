#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>

#include <time.h>
#include <unistd.h>

#include <string.h>
#include "int_list.h"


int convert_string_to_positive_number(char* string, int* number);

int generate_random_int(int min, int max);
long int generate_random_long();
void generate_unique_int(int* array, int count, int min, int max);