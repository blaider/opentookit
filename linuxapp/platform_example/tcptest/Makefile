ifndef CROSS_COMPILE
CROSS_COMPILE	:= arm-linux-gnueabihf-
endif

all:
	$(CROSS_COMPILE)gcc tcps.c -Wall -o tcps
	$(CROSS_COMPILE)gcc tcpc.c -Wall -o tcpc
	
clean:
	rm -rf udps udpc

.PHONY: all clean 