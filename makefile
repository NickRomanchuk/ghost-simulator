TARGETS = ghosthunt
OBJS = main.o rooms.o house.o hunters.o evidence.o ghosts.o utils.o logger.o
CC = gcc
CFLAGS = -Wextra -Wall

all: $(TARGETS)

ghosthunt: $(OBJS) defs.h
	$(CC) $(CFLAGS) $(OBJS) defs.h -o ghosthunt

main.o: main.c defs.h
	$(CC) $(CFLAGS) -c main.c

rooms.o: rooms.c defs.h
	$(CC) $(CFLAGS) -c rooms.c

house.o: house.c defs.h
	$(CC) $(CFLAGS) -c house.c

hunters.o: hunters.c defs.h
	$(CC) $(CFLAGS) -c hunters.c

evidence.o: evidence.c defs.h
	$(CC) $(CFLAGS) -c evidence.c

ghosts.o: ghosts.c defs.h
	$(CC) $(CFLAGS) -c ghosts.c

utils.o: utils.c defs.h
	$(CC) $(CFLAGS) -c utils.c

logger.o: logger.c defs.h
	$(CC) $(CFLAGS) -c logger.c

clean:
	rm -f $(TARGETS) $(OBJS)