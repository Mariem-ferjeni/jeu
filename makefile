enigme: main.o enigme.o
	gcc main.o enigme.o -o enigme -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm -g

main.o: main.c enigme.h
	gcc -c main.c -g `sdl2-config --cflags`

enigme.o: enigme.c enigme.h
	gcc -c enigme.c -g `sdl2-config --cflags`

clean:
	rm -f *.o enigme

run: enigme
	./enigme
