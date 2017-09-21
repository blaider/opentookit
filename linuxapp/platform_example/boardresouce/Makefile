ifndef CROSS_COMPILE
CROSS_COMPILE	:= arm-linux-gnueabihf-
endif

all:
	$(CROSS_COMPILE)gcc wdttest.c -Wall -o wdttest -lBoardResource -I. -L.
	$(CROSS_COMPILE)gcc nodeidtest.c -Wall -o nodeidtest -lBoardResource -I.  -L.
	$(CROSS_COMPILE)gcc ledtest.c -Wall -o ledtest -lBoardResource -I.  -L.
	$(CROSS_COMPILE)gcc diotest.c -Wall -o diotest -lBoardResource -I.  -L.
	$(CROSS_COMPILE)gcc batterytest.c -Wall -o batterytest -lBoardResource -I.  -L.
	
clean:
	rm -rf wdttest batterytest diotest ledtest nodeidtest

.PHONY: all clean 