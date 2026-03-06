compile: manager.cpp timer.cpp process.cpp launcher.cpp
	g++ -Wall -o manager manager.cpp
	g++ -Wall -o timer timer.cpp
	g++ -Wall -o process process.cpp
	g++ -Wall -o launcher launcher.cpp

run: compile
	./manager &
	sleep 0.5
	./timer &
	sleep 0.5
	./launcher

clean:
	rm -f manager timer process launcher
