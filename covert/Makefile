CC = gcc
CFLAGS = -w

all: covert-ff covert-fr covert-pp

covert-ff: covert-ff.c
	$(CC) $(CFLAGS) -o $@ $<

covert-fr: covert-fr.c
	$(CC) $(CFLAGS) -o $@ $<

covert-pp: covert-pp.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f covert-ff covert-fr covert-pp
