CC = gcc
CFLAGS = -c -std=gnu11 -g -Wall -fdiagnostics-color
LINK_FLAGS = -pthread -l sqlite3
LIBS_CLIENT = $(wildcard lib/client/*.c)
LIBS_SERVER = $(wildcard lib/server/*.c)
OBJECT_SERVER = $(wildcard build/server/*.o)
OBJECT_CLIENT = $(wildcard build/client/*.o)
BUILD_PATH = build/


link:compile
	@echo -e '\033[31m >>>[make]\033[32mLinking\033[31m SERVER objects\e[0m'
	$(CC) $(OBJECT_SERVER) $(LINK_FLAGS) -o build/server/server
	mv build/server/server bin/
	@echo -e '\033[31m >>>[make]\033[32mRemoving\033[31m SERVER objects\e[0m'
	rm -rf lib/server/*.o
	@echo -e '\033[31m >>>[make]\033[32mLinking\033[31m CLIENT objects\e[0m'
	$(CC) $(OBJECT_CLIENT) $(LINK_FLAGS) -o build/client/client
	mv build/client/client bin/
	@echo -e '\033[31m >>>[make]\033[32mRemoving\033[31m CLIENT objects\e[0m'
	rm -rf lib/client/*.o
compile:
	@clear screen
	@echo -e '\033[31m	>>> Make file for VirtualSoc ! <<<\e[0m\n\n'
	@echo -e '\033[31m >>>[make]\033[32mBuilding\033[31m SERVER objects\e[0m'
	$(CC) $(CFLAGS) $(LIBS_SERVER)
	mv *.o build/server/
	@echo -e '\033[31m >>>[make]\033[32mBuilding\033[31m CLIENT objects\e[0m'
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
