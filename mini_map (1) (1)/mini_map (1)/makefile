prog: main.o minimap.o
	gcc main.o minimap.o -o prog -lSDL2 -lSDL2_image -lSDL2_ttf -lm -g

main.o: main.c minimap.h
	gcc -c main.c -g `sdl2-config --cflags`

minimap.o: minimap.c minimap.h
	gcc -c minimap.c -g `sdl2-config --cflags`

clean:
	rm -f *.o prog
