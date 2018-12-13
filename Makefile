CLD_HOME=/home/abhishek/programs/clibdocker
#LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(CLD_HOME)/

CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl` -I/usr/include/iup -I$(CLD_HOME)
LIBS=$(CLD_HOME)/docker_*.so `pkg-config --libs json-c` `pkg-config --libs libcurl` -liupimglib -liup -liupcontrols -liupcd -lcd
DEBUG_FLAGS=-g -O0

all:		v-rex

v-rex:		main.o 
		$(CC) $(CFLAGS) -o v-rex main.o $(LIBS) $(DEBUG_FLAGS)

main.o: main.c
		$(CC) $(CFLAGS) -c -o main.o main.c $(LIBS) $(DEBUG_FLAGS)

clean:
		rm *.o v-rex
