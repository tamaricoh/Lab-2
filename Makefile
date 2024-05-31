CC = gcc
CFlags = -g -Wall -m32

Sources = LineParser.c myShell.c looper.c
OFiles = $(Sources:.c=.o)
Ex = lab2
ExLooper = looper

all: $(Ex) $(ExLooper)

$(Ex): $(OFiles)
	$(CC) $(CFlags) $(filter-out looper.o, $^) -o $@

$(ExLooper): looper.o
	$(CC) $(CFlags) $^ -o $@

%.o: %.c
	$(CC) $(CFlags) -c $< -o $@

clean:
	rm -f $(OFiles) $(Ex) $(ExLooper)