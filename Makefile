CC = gcc
CFLAGS = -Wall -Wextra -O2

SRCS = main.c game.c save.c
OBJS = $(SRCS:.c=.o)
TARGET = snake_game

ifeq ($(OS),Windows_NT)
	RM = del /Q
	EXE = .exe
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2
else
	RM = rm -f
	EXE =
	LDFLAGS = -lSDL2
endif

TARGET_BIN = $(TARGET)$(EXE)

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET_BIN) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJS) $(TARGET_BIN)

run: $(TARGET_BIN)
	$(TARGET_BIN)

.PHONY: all clean run