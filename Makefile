# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Generic Makefile for AVR C development
# @author Charlie Friend
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BUILD=./build
BASE_OUT=base
ROOMBA_OUT=roomba

CC=avr-gcc
OBJCOPY=avr-objcopy

MMCU=atmega2560

BASE_MAIN = main_base.c
BASE_ROOMBA = main_roomba.c

# All source files EXCEPT main
LIB_SOURCES = \
	tta.c \
	sched_timer.c \
	tta.c

SOURCES_BASE = $(LIB_SOURCES) $(BASE_MAIN)
SOURCES_ROOMBA = $(LIB_SOURCES) $(BASE_ROOMBA)

all: $(BUILD)/$(BASE_OUT).hex $(BUILD)/$(ROOMBA_OUT).hex

upload: $(BUILD)/$(BASE_OUT).hex $(BUILD)/$(ROOMBA_OUT).hex
	MMCU=$(MMCU) ./utils/program.py $(BUILD)/$(OUT).hex

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(BUILD)/$(BASE_OUT).elf: $(addprefix $(BUILD)/,$(SOURCES_BASE:.c=.o))
	$(CC) -mmcu=$(MMCU) -o $@ $^

$(BUILD)/$(ROOMBA_OUT).elf: $(addprefix $(BUILD)/,$(SOURCES_ROOMBA:.c=.o))
	$(CC) -mmcu=$(MMCU) -o $@ $^

$(BUILD)/%.o: %.c $(BUILD)
	$(CC) -mmcu=$(MMCU) -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)