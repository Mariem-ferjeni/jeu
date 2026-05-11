prog: main_puzzle.o puzzl.o
	gcc main_puzzle.o puzzl.o -o prog -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -g

main_puzzle.o: main_puzzle.c puzzl.h
	gcc -c main_puzzle.c -g `sdl2-config --cflags`

puzzl.o: puzzl.c puzzl.h
	gcc -c puzzl.c -g `sdl2-config --cflags`

clean:
	rm -f *.o prog
