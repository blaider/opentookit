ifndef CROSS_COMPILE
CROSS_COMPILE	:= arm-linux-gnueabihf-
endif

all:
	$(CROSS_COMPILE)gcc comtest.c -Wall -o comtest
	
clean:
	rm -rf comtest

.PHONY: all clean 