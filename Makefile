
.PHONY: monoprocessador multiprocessador clean

monoprocessador:
	gcc -Wall -pthread -g -Isrc -DNUM_CPUS=1 -o trabSO $(wildcard src/*.c)

multiprocessador:
	gcc -Wall -pthread -g -Isrc -DNUM_CPUS=2 -o trabSO $(wildcard src/*.c)

clean:
	rm -f trabSO
