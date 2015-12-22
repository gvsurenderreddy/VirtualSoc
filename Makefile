CC = gcc
CFLAGS = -c -std=gnu11 -g -Wall  -Wshadow -Wpointer-arith -Wcast-align -Wmissing-prototypes -Wmissing-declarations -Winline -Wlong-long -Wuninitialized -Wconversion -Wstrict-prototypes  -Wno-variadic-macros -l sqlite3
LINK_FLAGS =
LIBS_CLIENT = $(wildcard lib/client/*.c)
LIBS_SERVER = $(wildcard lib/server/*.c)
OBJECT = $(wildcard build/*.o)
BUILD_PATH = build/

link: compile
	cd build/
	$(CC) $(LINK_FLAGS) $(OBJECT) bin/server.o  -o bin/server
	$(CC) $(LINK_FLAGS) $(OBJECT) bin/client.o  -o bin/client
#	rm -rf bin/*.o
compile:
	$(CC) $(CFLAGS) $(LIBS_CLIENT)
	$(CC) $(CLFAGS) $(LIBS_SERVER)
	mv lib/client/*.o $(BUILD_PATH)
	#compile client and server objects
	$(CC) $(CFLAGS) server.c
	$(CC) $(CFLAGS) client.c
	mv *.o bin/
clean:
	rm -rf build/*.o
	rm -rf bin/*
