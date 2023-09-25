CC = gcc
CFLAGS = -g -Wall

SRC = main.c arff.c log.c
OBJ = $(SRC:.c=.o)

nomeDoPrograma = arff
all: $(nomeDoPrograma)

$(nomeDoPrograma): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o
purge:
	rm -rf *.o $(nomeDoPrograma)	
clean_relatorios:
	rm -rf R_* BLACKLIST.bl