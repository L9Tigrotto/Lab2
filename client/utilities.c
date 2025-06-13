#include "utilities.h"


void validate_arguments(int argc, char* argv[], int* number_of_servers, int* number_of_connections, int* number_of_messages) {

    // se gli argomenti sono 4 allora controllo che:
    // S sia un numero > 0.
    // K sia un numero 1 <= K < S.
    // M sia un numero > 3K

    if (argc != 4) {
        fprintf(stdout, "usage: %s S K M.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!convert_string_to_positive_number(argv[1], number_of_servers) || *number_of_servers == 0) {
        fprintf(stdout, "S must be a number greater than 0.\n");
        exit(EXIT_FAILURE);
    }

    if (!convert_string_to_positive_number(argv[2], number_of_connections) || *number_of_connections == 0 || *number_of_connections >= *number_of_servers) {
        fprintf(stdout, "K must be 1 <= K < S.\n");
        exit(EXIT_FAILURE);
    }

    if (!convert_string_to_positive_number(argv[3], number_of_messages) || *number_of_messages <= 3 * *number_of_connections) {
        fprintf(stdout, "M must be grater than 3k.\n");
        exit(EXIT_FAILURE);
    }
}

void generate(int* secret, long int* id) {
    *secret = generate_random_int(CLIENT_MIN_SECRET, CLIENT_MAX_SECRET);;
    *id = generate_random_long();

    snprintf(temp_text, TEMP_TEXT_LENGTH, "CLIENT %lX SECRET %d\n", *id, *secret);
    write_string_descriptor(1, temp_text);
}

int* connect_to_random_servers(int number_of_servers, int number_of_connections) {
    int connect_ports[number_of_connections];
    generate_unique_int(connect_ports, number_of_connections, 1, number_of_servers);

    int* socket_descriptors = (int*)malloc(sizeof(int) * number_of_connections);

    for (int i = 0; i < number_of_connections; i++) {
        struct sockaddr_in socket_addresses;
        create_socket_address(&socket_addresses, SERVER_IP, connect_ports[i] + 9000);
        socket_descriptors[i] = create_socket_descriptor();

        connect_to(socket_descriptors[i], &socket_addresses, sizeof(socket_addresses));
    }

    return socket_descriptors;
}

void send_messages(long int my_id, int secret, int* sockets, int count, int number_of_messages) {
    srand(time(NULL) * getpid());
    int random_index;
    long int converted_id = htobe64(my_id);

    for (int i = 0; i < number_of_messages; i++) {
        random_index = generate_random_int(0, count - 1);

        write(sockets[random_index], &converted_id, sizeof(long int));

        sleep_milliseconds(secret); 
    }
}

void close_connections(int* descriptors, int number_of_connections, long int my_id) {
    for (int i = 0; i < number_of_connections; i++) { close_descriptor(descriptors[i]); }
    free(descriptors);

    snprintf(temp_text, TEMP_TEXT_LENGTH, "CLIENT %lX DONE.\n", my_id);
    write_string_descriptor(1, temp_text);
}