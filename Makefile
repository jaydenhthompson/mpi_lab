CC = mpic++ 
SRCS = ./src/*.cpp
INC = ./src/
OPTS = -std=c++17 -Wall -Werror -O3
DEBUG_OPTS = -std=c++17 -Wall -Werror -g

EXEC = bin/barnes_hut

all: clean compile

debug: clean debug_compile

compile:
	$(CC) $(SRCS) $(OPTS) -I$(INC) -o $(EXEC)

debug_compile:
	$(CC) $(SRCS) $(DEBUG_OPTS) -I$(INC) -o $(EXEC)

clean:
	rm -f $(EXEC); mkdir -p bin
