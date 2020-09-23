BIN=bin/
SOURCE=src/
CC = gcc
CFLAGS = -Werror -Wall -I$(SOURCE)

PROG = cat chmod cp grep ls mkdir mv pwd rm
LIST=$(addprefix $(BIN), $(PROG))
make_dir = @mkdir -p $(@D)

all: $(LIST) shell

$(BIN)%: $(SOURCE)%.c
	$(make_dir)
	$(CC) $(CFLAGS) -o $@ $<

shell: $(SOURCE)neosh.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -r bin/
	rm shell
