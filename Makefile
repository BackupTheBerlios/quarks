
topdir	= .
subdirs	= lib kernel boot/$(arch) util

include $(topdir)/Makerules

checkout:
	cvs -d:pserver:anonymous@cvs.berlios.de:/cvsroot/quarks login <<\N
	cvs -z3 -d:pserver:anonymous@cvs.berlios.de:/cvsroot/quarks co .
