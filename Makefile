CC=gcc
CFLAGS=-std=c11 `pkg-config --cflags json-c` `pkg-config --cflags libcurl`
LIBS=`pkg-config --libs json-c` `pkg-config --libs libcurl`

all:		v-rex

v-rex:		main.o 
		$(CC) $(CFLAGS) -o v-rex main.o $(LIBS)

main.o: main.c
		$(CC) $(CFLAGS) -c -o main.o main.c $(LIBS)

clean:
		rm *.o v-rex
