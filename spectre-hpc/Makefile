CC=g++

CFLAGS = -w -std=c99 -no-pie

PROGRAM1 = spectre.out
SOURCE1  = spectre.cpp pfc.cpp

PROGRAM2 = benign_spectre.out
SOURCE2 = benign_spectre.cpp pfc.cpp
     
all: $(PROGRAM1) $(PROGRAM2)
     
$(PROGRAM1): $(SOURCE1) ; $(CC) $(CFLAGS) -o $(PROGRAM1) $(SOURCE1)

$(PROGRAM2): $(SOURCE2) ; $(CC) $(CFLAGS) -o $(PROGRAM2) $(SOURCE2)
     
clean: ; rm -f $(PROGRAM1) $(PROGRAM2)
