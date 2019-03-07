CC=gcc
CFLAGS += -Wall

# https://stackoverflow.com/questions/1704907/how-can-i-get-my-c-code-to-automatically-print-out-its-git-version-hash
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
CFLAGS += -DVERSION=\"$(GIT_VERSION)\"

CCOMMAND=$(CC) $(CFLAGS) $^ -o $@

attp-server : http.c
	$(CCOMMAND)

%.o: CFLAGS += -c
%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o attp-server