
topdir	= ..
srcdirs	= $(arch)
optimize	= -O2
warnings	= -Wall
cppflags	= -nostdinc -I$(topdir)/include -I$(arch)
cflags	= -no-fpic
cxxflags	= -no-fpic
ldflags	= --gc-sections
#objects	= quarks.o $(arch)/machine.o
targets	= kernel_$(arch)
crt0		= $(arch)/crt0.o
crtn		= $(arch)/crtn.o

include $(topdir)/Makerules
