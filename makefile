main: main.o SimSearcher.o
	g++ main.o SimSearcher.o -o main -g

SimSearcher.o:SimSearcher.cpp SimSearcher.h
	g++ SimSearcher.cpp -c
main.o:main.cpp SimSearcher.h
	g++ main.cpp -c

clean:
	rm main.o SimSearcher.o main