CC=g++
CFLAGS=-Wall -std=c++11

all: readckt

readckt: main.cpp
	$(CC) $(CFLAGS) main.cpp -o readckt

clean:
	rm -f readckt

test_readckt: readckt
	./readckt NLDM_lib_max2Inp c1908_.isc 1020 8 478

.PHONY: clean run

run: readckt
	@echo "Usage: ./readckt <file1> <file2> <int1> <int2> ..."
	@./readckt