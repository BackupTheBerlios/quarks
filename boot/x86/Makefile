
topdir	= ../..
optimize	= -O2
warnings	= -Wall
cppflags	= -nostdinc -I$(topdir)/include
cflags	= -no-fpic
ldflags	= -dN -Ttext 0x100000 -oformat binary
libs		= -L$(topdir)/lib/x86/console -L$(topdir)/lib -lconsole -lkern
#ldflags	= -nostdlib -Ttext 0x100000 -oformat binary $(topdir)/lib/x86/console/obj.x86/console.o
targets	= startup

include $(topdir)/Makerules
