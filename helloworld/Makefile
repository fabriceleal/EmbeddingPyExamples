TARGET=main
SRC=main.cpp
FLAGS=-I/usr/include/python2.7 -Wall -lpython2.7

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	g++ $(SRC) -o $(TARGET) $(FLAGS)

clean:
	rm -f $(TARGET)
