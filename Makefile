
# variables
COMPILER = gcc
FLAGS = -Wall
LINKS = -lrt -pthread


all: supervisor server client

test: all
	@ ./test.sh

clean:
	@ rm -f executable/* 

supervisor: executable/supervisor.o executable/supervisor_utilities.o executable/concurrent_queue.o executable/async.o executable/numbers.o executable/comunications.o executable/int_list.o executable/supervisor_concurrent_message_list.o
	@ echo "creating supervisor..."
	@ $(COMPILER) executable/supervisor.o executable/supervisor_utilities.o executable/concurrent_queue.o executable/async.o executable/numbers.o executable/comunications.o executable/int_list.o executable/supervisor_concurrent_message_list.o $(LINKS) -o executable/supervisor
	@ echo "done."

server: executable/server.o executable/concurrent_queue.o executable/server_utilities.o executable/server_concurrent_message_list.o executable/comunications.o executable/async.o executable/numbers.o executable/int_list.o executable/estimation_list.o
	@ echo "creating server..."
	@ $(COMPILER) executable/server.o executable/concurrent_queue.o executable/server_utilities.o executable/server_concurrent_message_list.o executable/comunications.o executable/async.o executable/numbers.o -o executable/server executable/int_list.o executable/estimation_list.o $(LINKS) -o executable/server
	@ echo "done."

client: executable/client.o executable/client_utilities.o executable/numbers.o executable/comunications.o executable/int_list.o executable/async.o
	@ echo "creating client..."
	@ $(COMPILER) executable/client.o executable/client_utilities.o executable/numbers.o executable/comunications.o executable/int_list.o executable/async.o $(LINKS) -o executable/client
	@ echo "done."





executable/concurrent_queue.o: shared/concurrent_queue.c shared/concurrent_queue.h
	@ $(COMPILER) $(FLAGS) -c -o executable/concurrent_queue.o shared/concurrent_queue.c

executable/int_list.o: shared/int_list.c shared/int_list.h
	@ $(COMPILER) $(FLAGS) -c -o executable/int_list.o shared/int_list.c

executable/numbers.o: shared/numbers.c shared/numbers.h
	@ $(COMPILER) $(FLAGS) -c -o executable/numbers.o shared/numbers.c

executable/async.o: shared/async.c shared/async.h
	@ $(COMPILER) $(FLAGS) -c -o executable/async.o shared/async.c

executable/comunications.o: shared/comunications.c shared/comunications.h
	@ $(COMPILER) $(FLAGS) -c -o executable/comunications.o shared/comunications.c





executable/supervisor.o: supervisor/supervisor.c supervisor/utilities.h
	@ $(COMPILER) $(FLAGS) -c -o executable/supervisor.o supervisor/supervisor.c

executable/supervisor_utilities.o: supervisor/utilities.c supervisor/utilities.h
	@ $(COMPILER) $(FLAGS) -c -o executable/supervisor_utilities.o supervisor/utilities.c

executable/supervisor_concurrent_message_list.o: supervisor/concurrent_message_list.c supervisor/concurrent_message_list.h
	@ $(COMPILER) $(FLAGS) -c -o executable/supervisor_concurrent_message_list.o supervisor/concurrent_message_list.c





executable/server.o: server/server.c server/utilities.h
	@ $(COMPILER) $(FLAGS) -c -o executable/server.o server/server.c

executable/server_utilities.o: server/utilities.c server/utilities.h
	@ $(COMPILER) $(FLAGS) -c -o executable/server_utilities.o server/utilities.c

executable/estimation_list.o: server/estimation_list.c server/estimation_list.h
	@ $(COMPILER) $(FLAGS) -c -o executable/estimation_list.o server/estimation_list.c

executable/server_concurrent_message_list.o: server/concurrent_message_list.c server/concurrent_message_list.h
	@ $(COMPILER) $(FLAGS) -c -o executable/server_concurrent_message_list.o server/concurrent_message_list.c





executable/client.o: client/client.c client/utilities.h
	@ $(COMPILER) $(FLAGS) -c -o executable/client.o client/client.c

executable/client_utilities.o: client/utilities.c client/utilities.h
	@ $(COMPILER) $(FLAGS) -c -o executable/client_utilities.o client/utilities.c


