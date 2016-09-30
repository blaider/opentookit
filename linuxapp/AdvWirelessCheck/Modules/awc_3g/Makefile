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

PROJECT_NAME	:= awc_3g
REVISION_NUMBER	:= $(shell svnversion -nc . | cut -f2 -d:)
VERSION_INFO	:= $(VERSION_NUMBER) rev $(REVISION_NUMBER)
PREDEFINE		:= _GNU_SOURCE $(DEBUG_FLAG) 
CFLAGS			:= -O -g -Wall -fPIC $(PREDEFINE:%=-D %)
LIBS			:= 
STATIC_LIBS		:= AwcCommon
USER_LIBS		:= xml2 z 
LDFLAGS			:= -lpthread -shared $(LIBS:%=-l%) $(USER_LIBS:%=-l%)  $(STATIC_LIBS:%=-l%)
TARGET			:= $(PROJECT_NAME).so

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
INC_DIR			:= -I"$(shell pwd)/../../Common" 
INC_DIR			+= -I"$(shell pwd)/../../../../thirdparty/libxml2/include/" 
endif

ifndef LIB_DIR
LIB_DIR			:= -L$(OUT_DIR)
endif

SRC_DIR			:= 
SRCS_C			:= awc_3g.c

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

$(OUT_DIR)/$(TARGET):$(OBJS) $(USER_LIBS:%=$(OUT_DIR)/lib%.so)  $(STATIC_LIBS:%=$(OUT_DIR)/lib%.a)
	@echo Building target: $@
	@$(MKDIR) -p $(OUT_DIR)
	@$(CROSS_COMPILE)gcc $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR) $(LIB_DIR) -Wl,-soname=$(TARGET) -Wl,-Map=$(OUT_DIR)/$(TARGET).map -o $(OUT_DIR)/$(TARGET) $(OBJS)  $(LDFLAGS) 
	@echo $(VERSION_INFO) > version_info.tmp
	@$(CROSS_COMPILE)objcopy --add-section version_number=version_info.tmp $(OUT_DIR)/$(TARGET)
	@$(RM) version_info.tmp
	@echo Building finished.


$(MY_OBJ_DIR)/%.o:%.c
	@echo Compiling: $<
	@$(MKDIR) -p $(MY_OBJ_DIR)/$(SRC_DIR)
	@$(CROSS_COMPILE)gcc -c $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(INC_DIR)  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"

$(OUT_DIR)/libAwcCommon.a:
	@cd $(shell pwd)/../../Common && make
	

$(OUT_DIR)/libxml2.so:
	@cp -f $(shell pwd)/../../../../thirdparty/libxml2/lib/armv7-linux-gcc/libxml2.so.2.8.0 $(OUT_DIR)/libxml2.so

$(OUT_DIR)/libz.so:
	@cp -f $(shell pwd)/../../../../thirdparty/libxml2/lib/armv7-linux-gcc/libz.so.1.2.7 $(OUT_DIR)/libz.so
clean:
	@$(RM) -r $(OBJ_DIR) $(OUT_DIR)/*.so*

.PHONY: all clean $(TARGET)
