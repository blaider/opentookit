include ../include/predefine.mk

PROJECT_NAME	:= libAwcCommon
SHARED_LIBS		:= xml2 z Expat
TARGET			:= $(PROJECT_NAME).a

INC_DIR			:= "$(shell pwd)/../libxml2/include"
INC_DIR			+= "$(shell pwd)/../expat/xmlparse"
INC_DIR			+= "$(shell pwd)/../include" 

SRCS_C			:= $(wildcard *.c)

include ../include/general_target.mk

all:$(TARGET)

$(OUT_DIR)/libxml2.so:
	@cp -f $(shell pwd)/../libxml2/lib/armv7-linux-gcc/libxml2.so.2.8.0 $(OUT_DIR)/libxml2.so

$(OUT_DIR)/libz.so:
	@cp -f $(shell pwd)/../libxml2/lib/armv7-linux-gcc/libz.so.1.2.7 $(OUT_DIR)/libz.so

$(OUT_DIR)/libExpat.so:
	@cd $(shell pwd)/../expat && make
	
clean:
	@$(RM) $(OBJS) $(DEPS) $(OUT_DIR)/$(TARGET)*

.PHONY: all clean $(TARGET)
