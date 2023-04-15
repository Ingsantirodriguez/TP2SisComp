CC = gcc
CFLAGS = -fdiagnostics-color=always -Wall -Wextra -std=c99 -g
LDFLAGS = -lcurl
ODIR = obj
IDIR = lib

all: converter

converter: $(ODIR)/converter.o $(ODIR)/cJSON.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(ODIR)/converter.o: converter.c $(IDIR)/cJSON.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/cJSON.o: $(IDIR)/cJSON.c $(IDIR)/cJSON.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f converter $(ODIR)/*.o output.json