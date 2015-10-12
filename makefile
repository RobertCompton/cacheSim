CC=gcc

cacheSim: cache.o main.o queue.o
	$(CC) -o cacheSim cache.o main.o queue.o

main.o: main.c
	$(CC) -c main.c

cache.o: cache.c
	$(CC) -c cache.c

queue.o: queue.c
	$(CC) -c queue.c

clean:
	rm -f *.o
