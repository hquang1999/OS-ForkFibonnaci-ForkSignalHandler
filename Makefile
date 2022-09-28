all: pa2fib pa2sig

pa2fib: PA02fib.o 
	gcc -o pa2fib PA02fib.o

PA02fib.o: PA02fib.c 
	gcc -c PA02fib.c

pa2sig: PA02sig.o
	gcc -o pa2sig PA02sig.o

PA02sig.o: PA02sig.c 
	gcc -c PA02sig.c

clean: 
	-rm *.o pa2fib pa2sig myfifo
