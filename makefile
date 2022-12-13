all:
	gcc -Wall -I /usr/include/ -o main *.cpp *.c -lSDL2 -lSDL2_mixer -lm -lGL -llua5.3

osx:
	clang++ -Wall -O3 \-F /Library/Frameworks/ -I /usr/local/include/ ./src/*.cpp ./src/*.c -o main -lm -framework OpenGL -framework SDL2 -framework SDL2_ttf

