CFLAGS_getDevice2_cop3.o := -I$(src)
obj-m +=getDevice2_copy3.o
#obj-m +=kmalloc2.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
