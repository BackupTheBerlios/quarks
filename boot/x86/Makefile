
topdir	= ../..
optimize	= -O2
warnings	= -Wall
cppflags	= -nostdinc -I$(topdir)/include -DBASE=$(base)
cflags	= -no-fpic -fomit-frame-pointer
ldflags	= -dN -Ttext 0 -oformat binary
libs		= -L$(topdir)/lib/x86/console -L$(topdir)/lib -L$(topdir)/lib/libzz -lconsole -lkern -lzz
targets	= startup
crt0		= bootstub.o
crt1		= startup.o
crtn		= end.o

include $(topdir)/Makerules
