CFLAGS := -g $(shell sdl2-config --cflags) -fno-pie
LDFLAGS := -no-pie

enigme: main.o enigme.o
	gcc main.o enigme.o -o enigme $(LDFLAGS) -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
	chmod +x enigme

main.o: main.c enigme.h
	gcc -c main.c $(CFLAGS)

enigme.o: enigme.c enigme.h
	gcc -c enigme.c $(CFLAGS)

clean:
	rm -f *.o enigme

run: enigme
	./enigme
