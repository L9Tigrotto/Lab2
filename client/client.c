#include "utilities.h"

int main(int argc, char* argv[]) {
    int number_of_servers, number_of_connections, number_of_messages, secret;
    long int id;

    validate_arguments(argc, argv, &number_of_servers, &number_of_connections, &number_of_messages);
    generate(&secret, &id);

    int* socket_descriptos = connect_to_random_servers(number_of_servers, number_of_connections);

    send_messages(id, secret, socket_descriptos, number_of_connections, number_of_messages);

    
    close_connections(socket_descriptos, number_of_connections, id);

    return 0;
}