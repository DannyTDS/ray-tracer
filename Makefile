CC=		gcc
CFLAGS=	-std=c++17 -Wall -Wextra -Iinclude -lstdc++

HEADERS=	$(wildcard include/*.h)

all: bin/main

bin/main:	src/main.cc $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f bin/main