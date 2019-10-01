BIN=./
SOURCE=./
CFLAGS=-ggdb -rdynamic -funwind-tables -pthread -lncurses
LIBS=engine.c 

LIST=human

all: $(LIST)

clean:
	-rm $(LIST)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%: $(SOURCE)%.c
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)

