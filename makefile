all: prog

crypfs.o: crypfs.h crypfs.c
	gcc crypfs.c -c -lm

test1: test1.o crypfs.o
	gcc test1.o crypfs.o -o test1 -lm
	./test1

test2: test2.o crypfs.o
	gcc test2.o crypfs.o -o test2 -lm
	./test2

test3: test3.o crypfs.o
	gcc test3.o crypfs.o -o test3 -lm
	./test3

test4: test4.o crypfs.o
	gcc test4.o crypfs.o -o test4 -lm
	./test4

test5: test5.o crypfs.o
	gcc test5.o crypfs.o -o test5 -lm
	./test5

test6: test6.o crypfs.o
	gcc test6.o crypfs.o -o test6 -lm
	./test6

test7: test7.o crypfs.o
	gcc test7.o crypfs.o -o test7 -lm
	./test7

test8: test8.o crypfs.o
	gcc test8.o crypfs.o -o test8 -lm
	./test8

tests3: tests3.o crypfs.o
	gcc tests3.o crypfs.o -o tests3 -lm
	./tests3

main.o: main.c crypfs.h
	gcc main.c crypfs.o -o main -lm

clean:
	rm -rf *.o
	rm -rf testes/*
	
mrproper: clean
	rm -rf crypfs
