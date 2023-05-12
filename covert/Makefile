CC = gcc
CFLAGS = -w

all: covert-ff covert-fr covert-pp

covert-ff: covert-ff.c
	$(CC) $(CFLAGS) -o $@ $< -lm

covert-fr: covert-fr.c
	$(CC) $(CFLAGS) -o $@ $< -lm

covert-pp: covert-pp.c
	$(CC) $(CFLAGS) -o $@ $< -lm

clean:
	rm -f covert-ff covert-fr covert-pp
