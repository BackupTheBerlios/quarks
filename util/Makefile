
# CPPFLAGS	= -I../include

all: mkifs 

mkifs: mkifs.o
	$(CC) -o $@ $< -L../lib/libzz -lzz 

clean:
	rm -f mkifs *.o
