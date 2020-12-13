
CC= gcc
CFLAGS = -Wall -g

all: p1_enc1_chan p2_enc2

p1_enc1_chan:p1_enc1_chan.o shared_create.o sema.o messg_struct.o rand_change.o
	$(CC) $(CFLAGS)  p1_enc1_chan.o shared_create.o sema.o messg_struct.o rand_change.o -o p1_enc1_chan -lssl -lcrypto

p2_enc2: p2_enc2.o shared_create.o sema.o messg_struct.o rand_change.o
	$(CC) $(CFLAGS)  p2_enc2.o shared_create.o sema.o messg_struct.o rand_change.o -o p2_enc2 -lssl -lcrypto

p1_enc1_chan.o: p1_enc1_chan.c
	$(CC) $(CFLAGS) -c p1_enc1_chan.c

p2_enc2.o: p2_enc2.c 
	$(CC) $(CFLAGS) -c p2_enc2.c

shared_create.o: shared_create.c shared_create.h messg_struct.h
	$(CC) $(CFLAGS) -c shared_create.c

sema.o: sema.c sema.h
	$(CC) $(CFLAGS) -c sema.c

rand_change.o: rand_change.c rand_change.h
	$(CC) $(CFLAGS) -c rand_change.c

messg_struct.o: messg_struct.c messg_struct.h
	$(CC) $(CFLAGS) -c messg_struct.c

clean:
	rm p1_enc1_chan p2_enc2 p1_enc1_chan.o p2_enc2.o shared_create.o sema.o rand_change.o messg_struct.o