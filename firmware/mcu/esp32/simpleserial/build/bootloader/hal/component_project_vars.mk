# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/hal/esp32/include $(IDF_PATH)/components/hal/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/hal -lhal
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += hal
COMPONENT_LDFRAGMENTS += $(IDF_PATH)/components/hal/linker.lf
component-hal-build: 
