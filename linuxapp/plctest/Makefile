ifndef CROSS_COMPILE
CROSS_COMPILE	:= arm-linux-gnueabihf-
endif

all:
	$(CROSS_COMPILE)gcc comtest.c -Wall -o comtestw1152o
	$(CROSS_COMPILE)gcc comtestr.c -Wall -o comtestr1152o
	$(CROSS_COMPILE)gcc comtestreadonly.c -Wall -o comtestreadonly
	
clean:
	rm -rf comtest

.PHONY: all clean 