src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -Ofast -m64 -s -static-libgcc 
LDFLAGS = -lSDL2 -lSDL2_ttf

breaky: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: winblows clean
winblows:
	x86_64-w64-mingw32-gcc $(CFLAGS) -DSDL_MAIN_HANDLED -Isdler/include/ -Lsdler/lib/ -Lsdttf/lib/ breaky.c -lSDL2 -lSDL2_ttf -o breaky.exe
clean:
	rm -f $(obj) pong