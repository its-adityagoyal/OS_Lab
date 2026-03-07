compile: manager.cpp nomad.cpp demon.cpp cond.c 
	gcc -c cond.c -o cond.o
	g++ -I. -Wall -o manager manager.cpp cond.o
	g++ -I. -Wall -o demon demon.cpp cond.o
	g++ -I. -Wall -o nomad nomad.cpp cond.o

run: compile
	./manager &
	sleep 0.5
	./demon 4 &
	./nomad 5

clean:
	rm -f manager demon nomad cond.o
