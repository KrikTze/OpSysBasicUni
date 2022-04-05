CC= gcc
CFLAGS = -Wall -g

2h:2h_trial.o filereader.o hexa_dec.o hash.o
	$(CC) $(CFLAGS) 2h_trial.o filereader.o hexa_dec.o hash.o -o 2h

2h.o: 2h_trial.c
	$(CC) $(CFLAGS) 2h_trial.c -c

filereader.o: filereader.c filereader.h 
	$(CC) $(CFLAGS) filereader.c  -c

hexa_dec.o:hexa_dec.c hexa_dec.h
	$(CC) $(CFLAGS) hexa_dec.c -c

hash.o:hash.c hash.h
	$(CC) $(CFLAGS) hash.c -c

clean:
	rm 2h 2h_trial.o filereader.o hexa_dec.o hash.o