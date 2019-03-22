include ../include/predefine.mk

PROJECT_NAME	:= libExpat
TARGET			:= $(PROJECT_NAME).so
VERSION_INFO	:=

INC_DIR			:= "$(shell pwd)/xmltok"
INC_DIR			+= "$(shell pwd)/xmlparse"
INC_DIR			+= "$(shell pwd)/xmlwf"

SRCS_C			:= xmlparse/xmlparse.c 
SRCS_C			+= xmltok/xmlrole.c
SRCS_C			+= xmltok/xmltok.c
SRCS_C			+= xmlwf/codepage.c
SRCS_C			+= xmlwf/readfilemap.c
SRCS_C			+= xmlwf/xmlfile.c
SRCS_C			+= xmlwf/xmlwf.c

include ../include/general_target.mk

all:$(TARGET)

clean:
	@$(RM) -r $(OBJ_DIR) $(OUT_DIR)/*.so*

.PHONY: all clean $(TARGET)
