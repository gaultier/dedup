.POSIX:

CFLAGS = -Wall -Wextra -Wpedantic -g -std=c99
CFLAGS_RELEASE = -O2
LDFLAGS = 
LIBS = 

.PHONY: clean

C_FILES= main.c
H_FILES= $(wildcard *.h)

dedup_debug: $(C_FILES) $(H_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(C_FILES) -o $@

dedup_release: $(C_FILES) $(H_FILES)
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS) $(LIBS) $(C_FILES) -o $@

clean:
	rm -f *.o dedup_debug dedup_release 
