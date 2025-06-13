#include "comunications.h"

void write_string_descriptor(int descriptor, char* string) {
    int length = strlen(string);

    if (write(descriptor, string, length) == -1) {
        perror("[comunications >> write_string_descriptor] error writing");
        exit(EXIT_FAILURE);
    }
}

int read_descriptor(int descriptor, size_t size, void* content) {
    int readed_bytes = read(descriptor, content, size);

    if (readed_bytes == -1) {
        perror("[comunications >> read_descriptor] error read");
        exit(EXIT_FAILURE);
    }

    return readed_bytes;
}

void close_descriptor(int descriptor) {
    if (close(descriptor) == -1) {
        perror("[comunications >> close_descriptor] error close");
        exit(EXIT_FAILURE);
    }
}

int open_shared_memory(char* name, int flags) {
    int shared_memory_descriptor = shm_open(name, flags, 0600);

    if (shared_memory_descriptor == -1) {
        perror("[comunications >> open_shared_memory] error opening shared memory");
        exit(EXIT_FAILURE);
    }

    return shared_memory_descriptor;
}

void truncate_shared_memory(int shared_memory_descriptor, size_t size) {
    if (ftruncate(shared_memory_descriptor, size) == -1) {
        perror("[comunications >> truncate_shared_memory] error truncating shared memory");
        exit(EXIT_FAILURE);
    }
}

void* map_shared_memory(int shared_memory_descriptor, size_t size) {
    void* pointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_descriptor, 0);
    
    if (pointer == MAP_FAILED) {
        fprintf(stderr, "[comunications >> map_shared_memory] error mapping shared memory.\n");
        exit(EXIT_FAILURE);
    }

    close(shared_memory_descriptor);
    return pointer;
}

void unmap_shared_memory(void* pointer, size_t size) {
    if (munmap(pointer, size) == -1) {
        perror("[comunications >> unmap_shared_memory] error unmapping shared memory");
        exit(EXIT_FAILURE);
    }
}

void unlink_shared_memory(char* name) {
    if (shm_unlink(name) == -1) {
        perror("[comunications >> unlink_shared_memory] error unlinking shared memory");
        exit(EXIT_FAILURE);
    }
}

void signal_process(pid_t process_id, int signal) {
    if (kill(process_id, signal) == -1) {
        perror("[comunications >> signal_process] error signaling process");
        exit(EXIT_FAILURE);  
    }
}

void wait_process(pid_t process_id) {
    int status;
    if (waitpid(process_id, &status, 0) == -1) {
        perror("[comunications >> wait_process] error waiting process");
        exit(EXIT_FAILURE);    
    }
}

void wait_thread(pthread_t thread_id) {
    if (pthread_join(thread_id, NULL) != 0) {
        perror("[comunications >> wait_thread] error joining thread");
        exit(EXIT_FAILURE);
    }
}


sigset_t create_new_thread_signal_mask(int* signals, int count) {
    sigset_t mask;
    if (sigemptyset(&mask) == -1) {
        perror("[comunications >> create_new_thread_signal_mask] error creating empty signal mask");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
        if (sigaddset(&mask, signals[i]) == -1) {
            perror("[comunications >> create_new_thread_signal_mask] error adding signal on mask");
            exit(EXIT_FAILURE);
        }
    }

    return mask;
}

void set_new_thread_signal_mask(sigset_t* mask) {
    if (pthread_sigmask(SIG_SETMASK, mask, NULL) == -1) {
        perror("[comunications >> set_new_thread_signal_mask] error setting signal mask");
        exit(EXIT_FAILURE);
    }
}

void wait_signal(sigset_t* mask) {
    int si;
    if (sigwait(mask, &si) != 0) {
        perror("[comunications >> wait_signal] error sigwait");
        exit(EXIT_FAILURE);
    }
}

int create_socket_descriptor() {
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_descriptor == -1) {
        perror("[comunications >> create_socket_descriptor] error creating socket");
        exit(EXIT_FAILURE);
    }

    return socket_descriptor;
}

void create_socket_address(struct sockaddr_in* socket_address,char* ip, int port) {
    socket_address->sin_family = AF_INET;
    socket_address->sin_port = htons(port);
    socket_address->sin_addr.s_addr = inet_addr(ip);
}

void bind_and_listen_socket(int socket, struct sockaddr_in* address, size_t size) {
    if (bind(socket, (struct sockaddr*)address, size) == -1) {
        perror("[comunications >> bind_and_listen_socket] error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(socket, SOMAXCONN) == -1) {
        perror("[comunications >> bind_and_listen_socket] error listening socket");
        exit(EXIT_FAILURE);
    }
}

int accept_client(int socket) {
    int client_descriptor = accept(socket, NULL, 0);

    if (client_descriptor == -1) {
        perror("[comunications >> accept_thread_routine] error accepting socket");
        exit(EXIT_FAILURE);
    }

    return client_descriptor;
}

void connect_to(int socket_descriptor, struct sockaddr_in* socket_address, size_t size) {
    if (connect(socket_descriptor, (struct sockaddr *)socket_address, size) == -1){
		perror("[comunications >> connect_to] error connecting to socket");
        exit(EXIT_FAILURE);
	}
}

void write_long_to_socket(int socket, long int* value) {
    if (write(socket, value, sizeof(long int)) == -1) {
        perror("[comunications >> write_long_to_socket] error writing");
        exit(EXIT_FAILURE);
    }
}

int select_read_with_timeout(fd_set* set, int max, int millisecond) {
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = millisecond * 1000;

    int result = select(max + 1, set, NULL, NULL, &timeout);
    if (result == -1) {
        perror("[comunications >> select_with_timeout] error select");
        exit(EXIT_FAILURE);
    }

    return result;
}