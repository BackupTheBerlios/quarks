
topdir	= ../..
optimize	= -O2
warnings	= -Wall
cppflags	= -nostdinc -I$(topdir)/include
cflags	= -no-fpic -fomit-frame-pointer
ldflags	= -dN -Ttext 0x100000 -oformat binary
libs		= -L$(topdir)/lib/x86/console -L$(topdir)/lib -lconsole -lkern
targets	= startup
crt0		= bootstub.o
crtn		= end.o

include $(topdir)/Makerules
