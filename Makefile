CC = gcc
CFLAGS = -c -std=gnu11 -g -Wall
LINK_FLAGS = -lpthread -lsqlite3
LIBS_CLIENT = $(wildcard lib/client/*.c)
LIBS_SERVER = $(wildcard lib/server/*.c)
OBJECT_SERVER = $(wildcard build/server/*.o)
OBJECT_CLIENT = $(wildcard build/client/*.o)
BUILD_PATH = build/


link:compile
	$(CC) $(LINK_FLAGS) $(OBJECT_SERVER) -o build/server/server
	mv build/server/server bin/
	# clean server stuff
	rm -rf lib/server/*.o
	#client
	$(CC) $(LINK_FLAGS) $(OBJECT_CLIENT) -o build/client/client
	mv build/client/client bin/
	#clean clinet stuff
	rm -rf lib/client/*.o
compile:
	#server
	$(CC) $(CFLAGS) $(LIBS_SERVER)
	mv *.o build/server/
	$(CC) $(CFLAGS) $(LIBS_CLIENT)
	mv *.o build/client/

clean:
	rm -rf *.o
	rm -rf build/server/*
	rm -rf build/client/*
	rm -rf bin/*

mem-server:
	valgrind ./bin/server 1
mem-client:
	valgrind ./bin/client 127.0.0.1 6047
