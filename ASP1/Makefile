CC = gcc
OBJS = ex1 ex2 ex3
TXT = output.txt

all: $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm $(OBJS) $(TXT)
