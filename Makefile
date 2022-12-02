CC = mpic++ 
SRCS = ./src/*.cpp
INC = ./src/
BASE = -std=c++17 -Wall -Werror -lglfw3 -lGL -lX11 -lXrandr -lXi -ldl -lpthread
OPTS = -O3
DEBUG_OPTS = -g

EXEC = bin/nbody

all: clean compile

debug: clean debug_compile

compile:
	$(CC) $(SRCS) $(BASE) $(OPTS) -I$(INC) -o $(EXEC)

debug_compile:
	$(CC) $(SRCS) $(BASE) $(DEBUG_OPTS) -I$(INC) -o $(EXEC)

clean:
	rm -f $(EXEC); mkdir -p bin
