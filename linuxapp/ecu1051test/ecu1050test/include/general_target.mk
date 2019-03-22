ifndef OBJ_DIR
OBJ_DIR			:= $(OUT_DIR)/obj
PRJ_OBJ_DIR		:= $(OBJ_DIR)
else
PRJ_OBJ_DIR		= $(OBJ_DIR:%=%/$(PROJECT_NAME))
endif

OBJS      		:= $(SRCS_C:%.c=$(PRJ_OBJ_DIR)/%.o)
OBJS      		+= $(SRCS_CXX:%.cpp=$(PRJ_OBJ_DIR)/%.o)
DEPS			:= $(OBJS:%.o=%.d)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

$(TARGET):$(OUT_DIR)/$(TARGET) $(EXTRA_TARGET)

$(OUT_DIR)/$(TARGET):$(OBJS) $(STATIC_LIBS:%=$(OUT_DIR)/lib%.a) $(SHARED_LIBS:%=$(OUT_DIR)/lib%.so)
	@echo Building target: $@
ifeq ($(suffix $(TARGET)),.so)
	$(__AT__)$(call link_shared_lib,$@,$(OBJS),$(TARGET))
else
ifeq ($(suffix $(TARGET)),.a)
	$(__AT__)$(call link_static_lib,$@,$(OBJS))
else
	$(__AT__)$(call link_exe_target,$@,$(OBJS),)
endif
endif
	@echo Building finished.

$(PRJ_OBJ_DIR)/%.o:%.c
	@echo Compiling: $<
	$(__AT__)$(call compile_c_files,$@,$<)

$(PRJ_OBJ_DIR)/%.o:%.cpp
	@echo Compiling: $<
	$(__AT__)$(call compile_cxx_files,$@,$<)


################################################################################
# macros

# $(1): output object file
# $(2): source code file
# $(3): additional macro define
define compile_c_files
	$(__AT__)$(MKDIR) $(dir $(1))
	$(__AT__)$(CROSS_COMPILE)gcc -c $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(3:%=-D %) $(INC_DIR:%=-I%)  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o $(1) $(2)
endef

# $(1): output object file
# $(2): source code file
# $(3): additional macro define
define compile_cxx_files
	$(__AT__)$(MKDIR) $(dir $(1))
	$(__AT__)$(CROSS_COMPILE)g++ -c $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(3:%=-D %) $(INC_DIR:%=-I%)  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o $(1) $(2)
endef

# $(1): target
# $(2): object files
define link_static_lib
	$(__AT__)$(MKDIR) $(dir $(1))
	$(__AT__)$(CROSS_COMPILE)ar -rcv $(1) $(2)
endef

# $(1): target
# $(2): object files
# $(3): soname
define link_shared_lib
	$(__AT__)$(MKDIR) $(dir $(1))
	$(__AT__)$(CROSS_COMPILE)gcc $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) -shared $(INC_DIR:%=-I%) $(LIB_DIR:%=-L%) -Wl,-soname=$(3) -Wl,-Map=$(1).map -o $(1) $(2) $(LDFLAGS)
	$(__AT__)$(if $(VERSION_INFO),$(call append_version_info,$(1)))
	$(__AT__)$(if $(DEBUG_FLAG:DEBUG=),$(CROSS_COMPILE)strip --strip-unneeded $(1))
endef

# $(1): target
# $(2): object files
# $(3): library
# $(4): additional macro define
define link_exe_target
	$(__AT__)$(MKDIR) $(dir $(1))
	$(__AT__)$(CROSS_COMPILE)gcc $(ARCH_FLAG) $(CPU_FLAG) $(CFLAGS) $(4:%=-D %) $(INC_DIR:%=-I%) $(LIB_DIR:%=-L%) -Wl,-Map=$(1).map -o $(1) $(2) $(LDFLAGS) $(3:%=-l%)
	$(__AT__)$(if $(VERSION_INFO),$(call append_version_info,$(1)))
	$(__AT__)$(if $(CAPSET),$(call append_capset,$(1)))
	$(__AT__)$(if $(DEBUG_FLAG:DEBUG=),$(CROSS_COMPILE)strip --strip-unneeded $(1))
endef

# $(1): target
define append_version_info
	$(__AT__)echo $(VERSION_INFO) > version_info.tmp
	$(__AT__)$(CROSS_COMPILE)objcopy --add-section version_number=version_info.tmp $(1)
	$(__AT__)$(RM) version_info.tmp
endef

# $(1): target
define append_capset
	$(__AT__)$(info append capset $(CAPSET) to $(1).)
	$(__AT__)echo $(CAPSET) > capset.tmp
	$(__AT__)$(CROSS_COMPILE)objcopy --add-section capset=capset.tmp $(1)
	$(__AT__)$(RM) capset.tmp
endef

.DEFAULT_GOAL	:= $(TARGET)
