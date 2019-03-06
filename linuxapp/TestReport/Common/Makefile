ifndef CROSS_COMPILE
CROSS_COMPILE	:= arm-linux-gnueabihf-
endif

ifndef ARCH_FLAG
ARCH_FLAG		:=
endif

ifndef CPU_FLAG
CPU_FLAG		:= -mcpu=cortex-a8
endif

ifndef VERSION_NUMBER
VERSION_NUMBER	:= 
endif

ifndef DEBUG_FLAG
DEBUG_FLAG		:= NDEBUG
endif

PROJECT_NAME	:= libAwcCommon
PREDEFINE		:= _GNU_SOURCE $(DEBUG_FLAG)
CFLAGS			:= -O -g -Wall -fPIC $(PREDEFINE:%=-D %)
LIBS			:=
SHARED_LIBS		:= 
LDFLAGS			:= -lpthread $(LIBS:%=-l%) $(SHARED_LIBS:%=-l%) $(STATIC_LIBS:%=-l%)
TARGET			:= $(PROJECT_NAME).a

ifndef OUT_DIR
OUT_DIR			:= $(shell pwd)/output
endif

ifndef OBJ_DIR
OBJ_DIR			:= $(OUT_DIR)/obj
MY_OBJ_DIR		:= $(OBJ_DIR)
else
MY_OBJ_DIR		:= $(OBJ_DIR:%=%/$(PROJECT_NAME))
endif

ifndef INC_DIR
INC_DIR			:= -I"$(shell pwd)/../../../thirdparty/libxml2/include" -I"$(shell pwd)/../../../thirdparty/expat/xmlparse" 
endif

ifndef LIB_DIR
LIB_DIR			:= -L$(OUT_DIR)
endif

SRC_DIR			:= 
SRCS_C			:= $(wildcard *.c)

OBJS      		:= $(SRCS_C:%.c=$(MY_OBJ_DIR)/%.o)
DEPS			:= $(OBJS:%.o=%.d)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

ifndef MKDIR
ifdef SystemRoot 						# for Windows sytem
MKDIR			:= mkdir.exe
else
MKDIR			:= mkdir
endif
endif

export CROSS_COMPILE
export ARCH_FLAG
export CPU_FLAG
export VERSION_NUMBER
export OUT_DIR
export OBJ_DIR
export DEBUG_FLAG

.DEFAULT_GOAL	:= $(TARGET)

all:$(TARGET)

$(TARGET):$(OUT_DIR)/$(TARGET)

$(OUT_DIR)/$(TARGET):$(OBJS) $(STATIC_LIBS:%=$(OUT_DIR)/lib%.a) $(SHARED_LIBS:%=$(OUT_DIR)/lib%.so)
	@echo Building target: $@
	@$(MKDIR) -p $(OUT_DIR)
	@$(CROSS_COMPILE)ar r $(OUT_DIR)/$(TARGET) $(OBJS)
	@echo Building finished.

$(MY_OBJ_DIR)/%.o:%.c
	@echo Compiling: $<
	@$(MKDIR) -p $(MY_OBJ_DIR)/$(SRC_DIR)
	@$(CROSS_COMPILE)gcc -c $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR)  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"

	
clean:
	@$(RM) $(OBJS) $(DEPS) $(OUT_DIR)/$(TARGET)*

.PHONY: all clean $(TARGET)
