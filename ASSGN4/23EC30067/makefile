cp: ludo.cpp
	g++ -Wall -std=c++17 -o ludo ludo.cpp

players: players.cpp
	g++ -Wall -std=c++17 -o players players.cpp

board: board.cpp
	g++ -Wall -std=c++17 -o board board.cpp

run: ludo players board
	./ludo 4 ludo.txt

clean:
	rm -f ludo players board
