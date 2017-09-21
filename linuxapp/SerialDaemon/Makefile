include ../../public/include/predefine.mk

PROJECT_NAME	:= serialdeamon
LIBS			:= 
SHARED_LIBS		:= 
TARGET			:= $(PROJECT_NAME)
CAPSET			:= "CAP_SYS_RAWIO=+eip"

INC_DIR			:= "$(shell pwd)/"
INC_DIR			+= "$(shell pwd)/../../sdk/APAL/include"
INC_DIR			+= "$(shell pwd)/../../sdk/BoardResourceSDK"

SRCS_C		:= serialdaemon_main.c
#SRCS_C		+= netinfo.c

include ../../public/include/general_target.mk

all:$(TARGET)

$(OUT_DIR)/libAPAL.so:
	@cd $(shell pwd)/../../sdk/APAL && make

clean:
	@$(RM) $(OBJS) $(DEPS) $(OUT_DIR)/$(TARGET)* $(SHARED_LIBS:%=$(OUT_DIR)/lib%.so*)

.PHONY: all clean $(TARGET)
