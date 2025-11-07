CC = gcc
CFLAGS = -g -Wall -DSET_MAX_TEMP=30 -DSET_MIN_TEMP=10

all: main.c datamgr.c datamgr.h config.h lib/dplist.c lib/dplist.h
	$(CC) $(CFLAGS) main.c datamgr.c lib/dplist.c lib/dplist.h -o main.out

gen: config.h file_creator.c 
	gcc -g file_creator.c config.h -o gen.out

gendebug: config.h file_creator.c
	gcc -g file_creator.c config.h -o gendebug.out -DDEBUG
