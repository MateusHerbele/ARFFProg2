CC = gcc
CFLAGS = -g -Wall

SRC = main.c
OBJ = $(SRC:.c=.o)

nomeDoPrograma = arff
all: $(nomeDoPrograma)

$(nomeDoPrograma): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o