CC = gcc
CFLAGS = -Wall -Wextra -O2
SDL_FLAGS = `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_mixer -lSDL2_ttf
TARGET = mon_jeu

all: $(TARGET)

$(TARGET): main_jeu.c level_manager.c
	$(CC) $(CFLAGS) -o $(TARGET) main_jeu.c level_manager.c $(SDL_FLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

rebuild: clean all
