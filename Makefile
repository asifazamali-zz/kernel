#CFLAGS_getDevice2_copy3.o := -I$(src)
#obj-m +=getDevice2_copy4.o
obj-m +=getDevice2_copy6.o
#obj-m +=kmalloc2.o
#obj-m+=demo3.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
