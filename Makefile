
topdir	= .
subdirs	= kernel

bootimage: $(subdirs)
	cp boot/bootsector bootimage
	cat kernel/kernel_x86 >> bootimage
	dd if=bootimage of=/boot/home/BeBochs1.2.1/quarks.img conv=notrunc

include $(topdir)/Makerules
