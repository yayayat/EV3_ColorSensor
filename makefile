BUILD_DIR = build
TARGET  ?= main

MCU     = stm8s103f3
ARCH    = stm8

F_CPU   ?= 2000000

LIBDIR   = ../lib

C_SRC    := $(wildcard src/*.c $(LIBDIR)/*.c)
ASM_SRC  := $(wildcard src/*.s $(LIBDIR)/*.s)

VPATH     = src $(LIBDIR)

C_INCLUDES =  \
-Iinc \
-I../lib 

OBJS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SRC:.c=.rel)))
OBJS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SRC:.s=.rel)))

CC       = sdcc
LD       = sdld
AS       = sdasstm8
OBJCOPY  = sdobjcopy
ASFLAGS  = -plosgff
CFLAGS   = -m$(ARCH) -p$(MCU) --std-sdcc11
CFLAGS  += -DF_CPU=$(F_CPU)UL $(C_INCLUDES)
CFLAGS  += --stack-auto --noinduction --use-non-free
LDFLAGS  = -m$(ARCH) -l$(ARCH) --out-fmt-ihx

all: size

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/$(TARGET).ihx: $(BUILD_DIR) $(OBJS)
	$(CC) $(LDFLAGS) $(addprefix $(BUILD_DIR)/, $(notdir $(OBJS))) -o $@
	
$(BUILD_DIR)/%.rel: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(BUILD_DIR)/$(notdir $@)

$(BUILD_DIR)/%.rel: %.s
	$(AS) $(ASFLAGS) $< -o $(BUILD_DIR)/$(notdir $@)

size: $(BUILD_DIR)/$(TARGET).ihx
	@$(OBJCOPY) -I ihex --output-target=binary $(BUILD_DIR)/$(TARGET).ihx $(TARGET).bin
	@echo "Image size:"
	@stat -L -c %s $(TARGET).bin


flash: size
	stm8flash -c stlinkv2 -p $(MCU) -w $(BUILD_DIR)/$(TARGET).ihx

clean:
	-rm -fR $(BUILD_DIR)
	-rm $(TARGET).bin

.PHONY: clean all flash
