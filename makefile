CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wfatal-errors -g
PROG = UnixLs
OBJS = UnixLs.o

UnixLs: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

UnixLs.o: UnixLs.c
	$(CC) $(CFLAGS) -c UnixLs.c 

clean:
	rm UnixLs.o