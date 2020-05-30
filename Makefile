src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -Ofast -m64 -s -static-libgcc 
LDFLAGS = -lSDL2 -lSDL2_ttf

breaky: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) pong