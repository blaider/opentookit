ifdef CROSS_COMPILE
#CC := $(CROSS_COMPILE)gcc
endif
all:
	$(CC) -Wall named_main.c -o named -lpthread
