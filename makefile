all:
	g++ -Wall -O3 -I /usr/include/ -fopenmp -o main ./src/*.cpp -lGLEW -lGLU -lGL -lSDL2 -lm 

osx:
	clang++ -Wall -O3 \-F /Library/Frameworks/ -I /usr/local/include/ -Xpreprocessor -fopenmp ./src/*.cpp ./src/*.c -o main -lm -lomp -framework OpenGL -framework SDL2 -framework SDL2_ttf



