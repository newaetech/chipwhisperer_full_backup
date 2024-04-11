# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/cbor/port/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/cbor -lcbor
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += $(IDF_PATH)/components/cbor/tinycbor
COMPONENT_LIBRARIES += cbor
COMPONENT_LDFRAGMENTS += 
component-cbor-build: 
