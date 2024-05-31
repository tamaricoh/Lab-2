CC = gcc
CFlags = -g -Wall -m32

Sources = LineParser.c myShell.c 
# Sources = looper.c
OFiles = $(Sources:.c=.o)
Ex = lab2

all: $(Ex)

$(Ex): $(OFiles)
	$(CC) $(CFlags) $(OFiles) -o $@

%.o: %.c
	$(CC) $(CFlags) -c $< -o $@

clean:
	rm -f $(OFiles) $(Ex)