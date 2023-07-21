PORT=58873
CFLAGS= -DPORT=\$(PORT) -g -std=gnu99 -Wall -Werror

all: friend_server friendme

friend_server: friend_server.o friends.o
	gcc ${CFLAGS} -o friend_server friend_server.o friends.o

friendme: friendme.o friends.o
	gcc ${CFLAGS} -o friendme friendme.o friends.o

friend_server.o: friend_server.c friends.h
	gcc ${CFLAGS} -o friend_server.o -c friend_server.c

friendme.o: friendme.c friends.h
	gcc ${CFLAGS} -o friendme.o -c friendme.c

friends.o: friends.c friends.h
	gcc ${CFLAGS} -o friends.o -c friends.c

clean:
	rm *.o friend_server friendme
