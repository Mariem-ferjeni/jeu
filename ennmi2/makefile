prog: main.o ennemi.o
	gcc main.o ennemi.o -o prog -lSDL2 -lSDL2_image -lm -g

main.o: main.c ennemi.h
	gcc -c main.c -g `sdl2-config --cflags`

ennemi.o: ennemi.c ennemi.h
	gcc -c ennemi.c -g `sdl2-config --cflags`

clean:
	rm -f *.o prog
