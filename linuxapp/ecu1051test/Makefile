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

SRC_CPP := main.cpp
SRC_CPP += Log.cpp
SRC_CPP += comtest.cpp
SRC_CPP += gprstest.cpp

all: $(OUT_DIR)/libAwcCommon.a
	@mkdir -p $(OUT_DIR)
	arm-linux-gnueabihf-g++ -Wall -o  $(OUT_DIR)/ecu1051test -I  $(INC_DIR) -I  $(INC_DIR1) -I  $(INC_DIR2) -g  $(SRC_CPP) -lpthread -lAwcCommon -lxml2 -ldl -lexpat -lrt -L $(LIB_DIR) 

$(OUT_DIR)/libAwcCommon.a:
	@cd $(shell pwd)/Common && make
	
$(OUT_DIR)/libExpat.so:
	@cd $(shell pwd)/expat && make

clean:
	rm   $(OUT_DIR)/ecu1051test
	
	
.PHONY: all clean
