CC = sdcc
CC_FLAGS = -mmcs51 --std-sdcc99
LD_FLAGS = --iram-size 128 --xram-size 0 --code-size 16384 --out-fmt-ihx
SHARED_FLAGS = --stack-auto

SRC_MAIN = main.c
SRC_FILES = $(SRC_MAIN) $(filter-out $(SRC_MAIN), $(wildcard *.c))
INC_FILES = $(wildcard *.h)
OUT_NAME = main.hex

SRC_DIR = ./
BUILD_DIR = build/

# ------------------------------------------------------------------------------

OBJ_FILES = $(SRC_FILES:.c=.rel)

SRC = $(addprefix $(SRC_DIR),$(SRC_FILES))
INC = $(addprefix $(SRC_DIR),$(INC_FILES))
OBJ = $(addprefix $(BUILD_DIR),$(OBJ_FILES))
OUT = $(addprefix $(BUILD_DIR),$(OUT_NAME))
BIN = $(OUT_NAME:.hex=.bin)

all: $(OBJ) $(INC)
	sdcc $(LD_FLAGS) $(SHARED_FLAGS) $(OBJ) -o $(OUT)
	mv $(OUT) $(OUT_NAME)
	objcopy -I ihex -O binary $(OUT_NAME) $(BIN)
	
$(BUILD_DIR)%.rel: $(SRC_DIR)%.c
	mkdir -p $(BUILD_DIR)
	sdcc $(CC_FLAGS) $(SHARED_FLAGS) -c $< -o $@
	
clean:
	rm -rf $(BUILD_DIR)*
	rm -rf *.hex *.ihx *.bin