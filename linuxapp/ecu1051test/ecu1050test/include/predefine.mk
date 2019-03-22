ifndef CROSS_COMPILE
CROSS_COMPILE	:= arm-linux-gnueabihf-
endif

ifndef ARCH_FLAG
ARCH_FLAG		:=
endif

ifndef CPU_FLAG
CPU_FLAG		:= -mcpu=cortex-a8 -mtune=cortex-a8 -march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon
endif

ifndef USER_NAME
USER_NAME		:= sysuser
endif

ifndef DEVICE_NAME
DEVICE_NAME		:= ADAM3600
endif

ifndef VERSION_NUMBER
VERSION_NUMBER	:=
endif

ifndef DEBUG_FLAG
DEBUG_FLAG		:= NDEBUG
endif

ifndef OUT_DIR
OUT_DIR			:= $(shell pwd)/output
endif

ifndef OBJ_DIR
OBJ_DIR			:= $(OUT_DIR)/obj
endif

ifndef LIB_DIR
LIB_DIR			:= $(OUT_DIR)
endif

ifeq ($(VERBOSE),1)
__AT__			:=
else
__AT__			:= @
endif


ifdef SystemRoot 						# for Windows sytem
MKDIR			?= mkdir.exe -p
HHC				?= hhc.exe
CMD_CAT			?= &
NUL				?= nul
QUOTE			?= "
else
MKDIR			?= mkdir -p
HHC				?= chmcmd
CMD_CAT			?= ;
NUL				?= /dev/null
QUOTE			?= \"
endif

MV				:= mv -f
RM				:= rm -rf
CP				:= cp -rf

export CROSS_COMPILE
export ARCH_FLAG
export CPU_FLAG
export USER_NAME
export DEVICE_NAME
export VERSION_NUMBER
export OUT_DIR
export OBJ_DIR
export DEBUG_FLAG

REVISION_NUMBER	:= $(shell git show -s --pretty=format:%h)
VERSION_INFO	:= $(VERSION_NUMBER) rev $(REVISION_NUMBER)
PREDEFINE		= _GNU_SOURCE $(DEBUG_FLAG) VERSION_NUMBER=\"$(VERSION_NUMBER)\" REVISION_NUMBER=\"$(REVISION_NUMBER)\"
CFLAGS			= -Os -g -Wall -fPIC -pthread $(PREDEFINE:%=-D %) $(DEVICE_NAME:%=-D %)
LDFLAGS			= -Wl,-rpath=/home/$(USER_NAME)/lib -Wl,-rpath=. -Wl,-rpath=$(OUT_DIR) -pthread $(LIBS:%=-l%) $(STATIC_LIBS:%=-l%) $(SHARED_LIBS:%=-l%)
