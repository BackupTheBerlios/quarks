
topdir	= .
subdirs	= lib kernel boot/$(arch) util

include $(topdir)/Makerules

qfs:
	util/mkifs util/quarks.ini -o quarks.qfs

floppy: qfs
	dd if=/dev/zero of=quarks.img bs=1024 count=1440
	dd if=boot/x86/bootblock of=quarks.img conv=notrunc
	dd if=quarks.qfs of=quarks.img seek=1 conv=notrunc

checkout:
	cvs -d:pserver:anonymous@cvs.berlios.de:/cvsroot/quarks login <<\N
	cvs -z3 -d:pserver:anonymous@cvs.berlios.de:/cvsroot/quarks co .
