obj-m:=drv.o
KERNELBUILD :=/lib/modules/$(shell uname -r)/build
default: 
	make -C $(KERNELBUILD) M=$(shell pwd) modules
	$(CC)  tdrv.c -o tdrv
	rm -rf *.mod.c  *.mod.o  *.o
	rm -rf .*.ko.cmd  .*.mod.o.cmd  .*.o.cmd  .tmp_versions
clean:
	rm -rf tdrv *.o *.ko *.mod.c .*.cmd *.markers *.order *.symvers .tmp_versions
