OS := $(shell uname)
LIBSLINUX = -lglfw3 -lGL -lX11 -lXxf86vm -lpthread -ldl -lXrandr -lXinerama -lXcursor -lm
LIBSMAC = -framework OpenGl -lglfw3
TARGET = ./bin/airport-renderer

objects = main.c

all: $(TARGET)

run: all
	$(TARGET)

$(TARGET): $(objects)
ifeq ($(OS),Darwin)
	$(CC) -o $@ $^ $(LIBSMAC)
else
ifeq ($(OS),Linux)
	$(CC) -o $@ $^ $(LIBSLINUX)
else
	echo "Not yet implemented windows compilation, attempting linux style command.";
	$(CC) -o $@ $^ $(LIBSLINUX)
endif
endif

clean:
	rm $(TARGET)



