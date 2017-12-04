LIBS = -lglfw3 -lGL -lX11 -lXxf86vm -lpthread -ldl -lXrandr -lXinerama -lXcursor -lm

LDFLAGS = $(LIBS)
TARGET = ./bin/airport-renderer

objects = main.c

all: $(TARGET)

$(TARGET): $(objects) 
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm $(TARGET)
