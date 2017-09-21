ifndef CROSS_COMPILE
CROSS_COMPILE	:=
endif

all:
	gcc tcps.c -Wall -o tcps -lpthread
	gcc tcpc.c -Wall -o tcpc -lpthread
	
clean:
	rm -rf udps udpc

.PHONY: all clean 