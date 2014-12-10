ifdef CROSS_COMPILE
CC 				:= $(CROSS_COMPILE)gcc
CXX 			:= $(CROSS_COMPILE)g++
AR 				:= $(CROSS_COMPILE)ar
endif

ifndef ARCH_FLAG
ARCH_FLAG		:=
endif

ifndef CPU_FLAG
CPU_FLAG		:=
endif

PROJECT_NAME	:= mktest
PREDEFINE		:= _GNU_SOURCE NDEBUG
CFLAGS			:= -O -g -Wall $(PREDEFINE:%=-D %)
LIBS			:=
SHARED_LIBS		:=
STATIC_LIBS		:=
LDFLAGS			:= $(LIBS:%=-l%) $(SHARED_LIBS:%=-l%) $(STATIC_LIBS:%=-l%)
TARGET			:= $(PROJECT_NAME)

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
INC_DIR			:= -I"$(shell pwd)/../../include" -I"$(shell pwd)/../../../../thirdparty/libxml2/include/" 
endif

ifndef LIB_DIR
LIB_DIR			:= -L$(OUT_DIR)
endif

SRC_DIR			:= 
SRCS_C			:= $(wildcard *.c)
SRCS_CPP		:= $(wildcard *.cpp)

OBJS_C      	:= $(SRCS_C:%.c=$(MY_OBJ_DIR)/%.o)
OBJS_CPP      	:= $(SRCS_CPP:%.cpp=$(MY_OBJ_DIR)/%.o)
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
export OUT_DIR
export OBJ_DIR

.DEFAULT_GOAL	:= $(TARGET)

all:$(TARGET)

$(TARGET):$(OUT_DIR)/$(TARGET)

$(OUT_DIR)/$(TARGET):$(OBJS_C) $(OBJS_CPP) $(STATIC_LIBS:%=$(OUT_DIR)/lib%.a) $(SHARED_LIBS:%=$(OUT_DIR)/lib%.so)
	@echo Building target: $@
	@$(MKDIR) -p $(OUT_DIR)
	$(CXX) $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR) $(LIB_DIR) -Wl,-Map=$(OUT_DIR)/$(TARGET).map -o $(OUT_DIR)/$(TARGET) $(OBJS_C) $(OBJS_CPP) $(LDFLAGS)
	@echo Building finished.

$(MY_OBJ_DIR)/%.o:%.cpp
	@echo Compiling: $<
	echo $(CXX)
	@$(MKDIR) -p $(MY_OBJ_DIR)/$(SRC_DIR)
	$(CXX) -c $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR)  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"

$(MY_OBJ_DIR)/%.o:%.c
	@echo Compiling: $<
	@$(MKDIR) -p $(MY_OBJ_DIR)/$(SRC_DIR)
	$(CC) -c $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR)  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"

clean:
	@$(RM) $(OBJS) $(DEPS) $(OUT_DIR)/$(TARGET)*

.PHONY: all clean $(TARGET)
