prog: main.o background.o
	gcc main.o background.o -o prog -lm -g -lSDL2 -lSDL2_image -lSDL2_ttf

main.o: main.c background.h
	gcc -c main.c -g `sdl2-config --cflags`

background.o: background.c background.h
	gcc -c background.c -g `sdl2-config --cflags`

clean:
	rm -f *.o prog
