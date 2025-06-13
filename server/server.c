// ################# server ################# 
// 
// il server riceve da input [ "server", "i", NULL ] con i un numero da 1 a S.
// 
// 1) controllo che il parametro i sia un numero e che sia maggiore di 0.
// 
// 2) apro il segmento di memoria condiviso e lo mappo.
// 
// 3) creo e apro la socket con ip 127.0.0.1 (localhost) e porta 9000 + i.
// 
// 4) creo un thread che accetta le connessioni e tre thread che gestiscono i server,
//    così quello principale lo utilizzo per gestire i segnali.
// 
// x) aspetto finchè un SIGINT non viene catturato e procedo a chiudere gli altri thread.
// 
// 
// 
// ---------- thread gestore del segnale ----------
// 
// aspetta un sengale SIGINT con sigwait. quando arriva chiude il socket
// così che il thread che accetta i client esce dall'accpt l'accept con ENOTSOCK
// e la flag stop a 1.
// 
// 
// 
// 
// ---------- thread che accetta i client ----------
// 
// fa un loop infinito nel quale accetta le connessioni dei client finchè non riceve
// l'errore EBADF, a quel punto esce dal loop e termina.
// 
// dopo aver accettato un cliente mette il file descriptor del cliente nel set,
// così che i thread che gestiscono i clienti possono prenderlo con la select.
// 
// 
// 
// 
// ---------- thread che gestisce i client ----------
// 
// fa un loop infinito nel quale controlla nella select se ci sono client disponibili. 
// se ci sono client allora li gestisce, altrimenti aspetta per un massimo di check ms e
// controlla se il flag stop è stato settato a 1, se si allora si ferma.

#include "utilities.h"

int main(int argc, char* argv[]) {

    // 1)
    validate_arguments(argc, argv, &server_id);

    // 3)
    concurrent_queue = open_and_map_shared_memory(SHARED_MEMORY_NAME);

    // 4)
    socket_descriptor = open_socket(SERVER_IP, (9000 + server_id));

    // 5)
    create_threads(&accept_thread, manager_threads, MANAGER_COUNT);

    handle_signals();
    return 0;
}