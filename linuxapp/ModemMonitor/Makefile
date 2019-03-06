ifndef OUT_DIR
OUT_DIR			:= $(shell pwd)/output
endif

ifndef LIB_DIR
LIB_DIR			:= $(shell pwd)/Common/output
endif

ifndef INC_DIR
INC_DIR			:= $(shell pwd)/Common
INC_DIR1			:= $(shell pwd)/libxml2/include/
INC_DIR2			:= $(shell pwd)/include/
endif

SRC_C := modem_monitor_main.c
SRC_C += Log.c
SRC_C += modem_scan.c
SRC_C += led.c


all: 
	@mkdir -p $(OUT_DIR)
	arm-linux-gnueabihf-gcc -Wall -o  $(OUT_DIR)/modemmonitor -I  $(INC_DIR) -I  $(INC_DIR1) -I  $(INC_DIR2) -g  $(SRC_C) -lpthread -L $(LIB_DIR) 

clean:
	rm   $(OUT_DIR)/modemmonitor
	
	
.PHONY: all clean
