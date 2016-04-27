CC=g++
CFLAGS=-`pkg-config opencv --cflags` `pkg-config opencv --libs`
SOURCES=main.cpp Image.cpp SubImage.cpp CompressedImage.cpp
EXECUTABLE=compress_image

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(SOURCES) -o $(EXECUTABLE) `pkg-config --cflags --libs opencv` -std=gnu++11

clean:
	rm $(EXECUTABLE)


