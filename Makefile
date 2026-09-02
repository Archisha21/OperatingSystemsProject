CC=gcc
CFLAGS=-Wall -Wextra -O2
LIBS=-lpsapi
TARGET=predictive_framework.exe

SRC=main.c process.c predict.c control.c data.c
OBJ=$(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJ) $(TARGET) process_history.csv 2>NUL

run: $(TARGET)
	./$(TARGET)
