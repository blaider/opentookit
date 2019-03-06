ifndef OUT_DIR
OUT_DIR			:= $(shell pwd)/output
endif

ifndef LIB_DIR
LIB_DIR			:= $(shell pwd)/lib
endif

ifndef INC_DIR
INC_DIR			:= $(shell pwd)/inc
endif

SRC_CPP := main.cpp
SRC_CPP += Log.cpp
#SRC_CPP += comtest.cpp
#SRC_CPP += aoai.cpp
SRC_CPP += moduleinfo.cpp
SRC_CPP += dio.cpp

all:
	@mkdir -p $(OUT_DIR)
	arm-linux-gnueabihf-g++ -Wall -o  $(OUT_DIR)/adam5080test  -g  $(SRC_CPP) -I$(INC_DIR) -lbio5630io -lpthread -L$(LIB_DIR)

clean:
	rm   $(OUT_DIR)/adam5630test
	
	
.PHONY: all clean
