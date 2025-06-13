// ################# supervisor ################# 
// 
// ---------- main ----------
//
// il supervisor riceve da input [ "supervisor", "S", NULL ]
// con S il numero di processi server da lanciare.
// 
// 1) controllare che il parametro S sia un numero e che sia maggiore di zero.
// 
// 2) come da comando scrivere su standard output "supervisor starting {S} server".
// 
// 3) creare in modo esclusivo il segmento di memoria condiviso della grandezza
//    sizeof(concurrent_queue_t), lo mappo e inizializzo concurrent_queue_t.
// 
// 4) faccio partire gli S server come processi distinti. creo una copia del processo
//    attuale ma gli faccio fare execl di server subito dopo.
// 
// 5) creo un thread separato per leggere dalla memoria condivisa, cosi con quello
//    principale posso gestire i segnali.
//
// x) aspetto finchè un doppio SIGINT non viene catturato in meno di un secondo e procedo
//    a chiudere supervisor e server e a deallocare le risorse.
// 
// 
//
// ---------- thread gestore del segnale ----------
//
// aspetta un segnale con sigwait, se SIGINT è "unico" allora viene stampata una tabella
// con tutti i risultati su standard error, altrimenti se viene catturato un secondo SIGINT
// in meno di un secondo dal precedente viene terminato il supervisor e tutti i server. 
//
// invoco concurrent_queue_disable_request così da non permettere a nessuno di scrivere
// nella coda con concurrent_queue_enqueue ma di permettere di leggere finchè la coda non
// sarà vuota, a quel punto il thread lettore teminarà.
//
// per terminare i server invio un SIGINT ad ogni server, il thread che si occupa di gestire
// i segnali di quel server interromperà gli altri thread e si chiuderà cosi il processo.
// dopo aver inviato i segnali SIGINT ai server aspetto la loro fine son con pthread_wait.
// 
// quindi il thread gestore del segnale invia i segnali SIGINT ai server, invoca
// concurrent_queue_disable_request e aspetta il termine dei server con wait_pid e anche il
// thread lettore con pthread_join, a quel punto è necessario deallocare tutte le risorse.
// 
//
//
// ---------- thread lettore ----------
// 
// imposto l'opzione per ignorare il segnale SIGINT così che sono sicuro che verrà gestito
// dal thread gestore del segnale.
//
// continua a leggere record dalla coda concorrente.
// - se il record è un record normale, controllo se è il primo messaggio da parte di quel client.
//      - se è il primo messaggio aggiungo il record al dizionario
//        { nuovo_client_id, [{nuova_stima, 1}] }
//      - se client_id è già presente nel dizionario devo controllare se una stima uguale
//        è già stata effettuata
//          - se una stima uguale è già stata fatta allora aumento il contatore di quella stima 
//            { vecchio_client_id, [..., {vecchia_stima, times++}] }.
//          - se non è presente una stima uguale, la aggiungo
//            { vecchio_client_id, [..., {nuova_stima, 1}] }.
// - se viene restituito CONCURRENT_QUEUE_DEQUEUE_DISABLE termino il thread.

#include "utilities.h"

int main(int argc, char* argv[]) {
    // 1)
    validate_arguments(argc, argv, &number_of_servers);

    // 3)
    concurrent_queue = open_exclusive_and_map_shared_memory(SHARED_MEMORY_NAME);

    // 4)
    server_processes = (pid_t*)malloc(sizeof(pid_t) * number_of_servers);
    fork_and_exec_servers(server_processes, number_of_servers);

    // 5)
    create_read_thread(&read_thread);

    handle_signals();
}



