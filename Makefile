CC = gcc
ASM = nasm
CFLAGS = -fdiagnostics-color=always -Wall -Wextra -std=c99 -g -m32
ASMFLAGS = -f elf32
LDFLAGS = -lcurl
ODIR = obj
IDIR = lib

all: converter

converter: $(ODIR)/converter.o $(ODIR)/cJSON.o $(ODIR)/convert.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ODIR)/converter.o: converter.c $(IDIR)/cJSON.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/cJSON.o: $(IDIR)/cJSON.c $(IDIR)/cJSON.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/convert.o: convert.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f converter $(ODIR)/*.o output.json