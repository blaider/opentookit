ifndef CROSS_COMPILE
CROSS_COMPILE	:= arm-linux-gnueabihf-
endif

all:
	$(CROSS_COMPILE)gcc udps.c -Wall -o udps
	$(CROSS_COMPILE)gcc udpc.c -Wall -o udpc
	
clean:
	rm -rf udps udpc

.PHONY: all clean 