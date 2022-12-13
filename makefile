all:
	g++ -Wall -O3 -I /usr/include/ -o main ./src/*.cpp ./src/*.c -lSDL2 -lSDL2_ttf  -lm

osx:
	clang++ -Wall -O3 \-F /Library/Frameworks/ -I /usr/local/include/ ./src/*.cpp ./src/*.c -o main -lm -framework OpenGL -framework SDL2 -framework SDL2_ttf

