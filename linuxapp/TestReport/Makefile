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

PROJECT_NAME	:= testReport
REVISION_NUMBER	:= $(shell svnversion -nc . | cut -f2 -d:)
VERSION_INFO	:= $(VERSION_NUMBER) rev $(REVISION_NUMBER)
PREDEFINE		:= _GNU_SOURCE $(DEBUG_FLAG) VERSION_NUMBER=\"$(VERSION_NUMBER)\" REVISION_NUMBER=\"$(REVISION_NUMBER)\"
CFLAGS			:= -O -g -Wall $(PREDEFINE:%=-D%)
LIBS			:= dl
STATIC_LIBS		:= AwcCommon
SHARED_LIBS		:= 
LDFLAGS			:= -lpthread $(LIBS:%=-l%) $(SHARED_LIBS:%=-l%) $(STATIC_LIBS:%=-l%)
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
INC_DIR			:= -I"$(shell pwd)/../../thirdparty/libxml2/include/" 
INC_DIR			+= -I"$(shell pwd)/../../sdk/DCTag"
INC_DIR			+= -I"$(shell pwd)/Common"
endif

ifndef LIB_DIR
LIB_DIR			:= -L$(OUT_DIR)
endif

SRC_DIR			:= 
SRCS_C			:= $(wildcard *.cpp) 

OBJS      		:= $(SRCS_C:%.cpp=$(MY_OBJ_DIR)/%.o)
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
export DEBUG_FLAG

.DEFAULT_GOAL	:= $(TARGET)

all:$(TARGET)

$(TARGET):$(OUT_DIR)/$(TARGET)

$(OUT_DIR)/$(TARGET):$(OBJS) $(STATIC_LIBS:%=$(OUT_DIR)/lib%.a) $(SHARED_LIBS:%=$(OUT_DIR)/lib%.so)
	@echo Building target: $@
	@$(MKDIR) -p $(OUT_DIR)
	@$(CROSS_COMPILE)g++ $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR)  $(LIB_DIR) -Wl,-Map=$(OUT_DIR)/$(TARGET).map -o $(OUT_DIR)/$(TARGET) $(OBJS) $(LDFLAGS)
	@echo $(VERSION_INFO) > version_info.tmp
	@$(CROSS_COMPILE)objcopy --add-section version_number=version_info.tmp $(OUT_DIR)/$(TARGET)
	@$(RM) version_info.tmp
	@echo Building finished.
	@cd $(shell pwd)/Modules && make

$(MY_OBJ_DIR)/%.o:%.cpp
	@echo Compiling: $<
	@$(MKDIR) -p $(MY_OBJ_DIR)/$(SRC_DIR)
	@$(CROSS_COMPILE)g++ $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR)  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"

$(OUT_DIR)/libAwcCommon.a:
	@cd $(shell pwd)/Common && make


clean:
	@$(RM) $(OBJS) $(DEPS) $(OUT_DIR)/$(TARGET)* $(SHARED_LIBS:%=$(OUT_DIR)/lib%.so*)

.PHONY: all clean $(TARGET)
