# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Generic Makefile for AVR C development
# @author Charlie Friend
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BUILD=./build
CONTROLLER_OUT=controller
ROOMBA_OUT=roomba

CC=avr-gcc
OBJCOPY=avr-objcopy

MMCU=atmega2560

ROOMBA_MAIN = main_roomba.c
CONTROLLER_MAIN = main_controller.c

# All source files EXCEPT main
LIB_SOURCES = \
	tta.c \
	sched_timer.c \
	tta.c

SOURCES_CONTROLLER = $(LIB_SOURCES) $(CONTROLLER_MAIN)
SOURCES_ROOMBA = $(LIB_SOURCES) $(ROOMBA_MAIN)

all: $(BUILD)/$(CONTROLLER_OUT).hex $(BUILD)/$(ROOMBA_OUT).hex

upload_controller: $(BUILD)/$(CONTROLLER_OUT).hex
	MMCU=$(MMCU) ./utils/program.py $<

upload_roomba: $(BUILD)/$(ROOMBA_OUT).hex
	MMCU=$(MMCU) ./utils/program.py $<

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(BUILD)/$(CONTROLLER_OUT).elf: $(addprefix $(BUILD)/,$(SOURCES_CONTROLLER:.c=.o))
	$(CC) -mmcu=$(MMCU) -o $@ $^

$(BUILD)/$(ROOMBA_OUT).elf: $(addprefix $(BUILD)/,$(SOURCES_ROOMBA:.c=.o))
	$(CC) -mmcu=$(MMCU) -o $@ $^

$(BUILD)/%.o: %.c $(BUILD)
	$(CC) -mmcu=$(MMCU) -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)