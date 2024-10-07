HEADERS = enums.h structs.h devices.h
SOURCE = main.c
BIN_NAME = zigc
CC = gcc
CFLAGS = -Wall -Wextra -lbsd

$(BIN_NAME) : $(SOURCE) $(HEADERS)
	$(CC) -o $(BIN_NAME) $(SOURCE) $(CFLAGS)

.PHONY : clean
clean :
	rm *.o $(BIN_NAME)
.PHONY : intclean
intclean :
	rm *.o
