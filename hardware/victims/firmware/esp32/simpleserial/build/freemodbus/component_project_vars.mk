# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/freemodbus/modbus/include $(IDF_PATH)/components/freemodbus/modbus_controller
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/freemodbus -lfreemodbus
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += freemodbus
COMPONENT_LDFRAGMENTS += 
component-freemodbus-build: 
