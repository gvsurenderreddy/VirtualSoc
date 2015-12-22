CC = gcc
CFLAGS = -c -std=gnu11 -g -Wall  -l sqlite3
LINK_FLAGS = -lpthread
LIBS_CLIENT = $(wildcard lib/client/*.c)
LIBS_SERVER = $(wildcard lib/server/*.c)
OBJECT = $(wildcard build/*.o)
BUILD_PATH = build/


all: 
	#$(CC) $(CFLAGS) $(LIBS_CLIENT)
	$(CC) $(CFLAGS) $(LIBS_SERVER)
	mv *.o $(BUILD_PATH)
	cd build
	$(CC) $(LINK_FLAGS) $(OBJECT) -o server
	mv server bin/

clean:
	rm -rf *.o
	rm -rf build/*.o
