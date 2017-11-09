all: src/*.cpp src/*.h
	g++ src/*.cpp -o build/main -std=c++11