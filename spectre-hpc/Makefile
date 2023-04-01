CC=g++

CFLAGS = -std=c99

PROGRAM = spectre.out
SOURCE  = spectre.cpp pfc.cpp
     
all: $(PROGRAM)
     
$(PROGRAM): $(SOURCE) ; $(CC) -o $(PROGRAM) $(SOURCE) -no-pie
     
clean: ; rm -f $(PROGRAM)
