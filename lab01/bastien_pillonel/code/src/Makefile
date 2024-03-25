CC=gcc
CFLAGS= -O3 -g -Wall -I../../lib/stb/ -I../include -fno-inline
LDFLAGS= -lm
DEBUGFLAGS= -std=c11 -Wall -Wextra -pedantic -g -I../../lib/stb/ -I../include

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
BIN=lab01
HEADERS = $(wildcard *.h)

all: $(BIN)

debug: $(SRC_C)
	$(CC) $(DEBUGFLAGS) -o $(BIN) $^ ${LDFLAGS}

$(BIN): $(OBJECTS)
	$(CC) -o $(BIN) $^ ${LDFLAGS}

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clean:
	rm -f *.o
	rm $(BIN)

.PHONY : all
.PHONY : debug
.PHONY : clean