CC=g++

CFLAGS = -std=c99

PROGRAM1 = spectre.out
SOURCE1  = spectre.cpp pfc.cpp

PROGRAM2 = benign_spectre.out
SOURCE2 = benign_spectre.cpp pfc.cpp
     
all: $(PROGRAM1) $(PROGRAM2)
     
$(PROGRAM1): $(SOURCE1) ; $(CC) -o $(PROGRAM1) $(SOURCE1) -no-pie

$(PROGRAM2): $(SOURCE2) ; $(CC) -o $(PROGRAM2) $(SOURCE2) -no-pie
     
clean: ; rm -f $(PROGRAM1) $(PROGRAM2)
