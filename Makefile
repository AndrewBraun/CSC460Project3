# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Generic Makefile for AVR C development
# @author Charlie Friend
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Q=@

BUILD=./build
CONTROLLER_OUT=controller
ROOMBA_OUT=roomba

CC=avr-gcc
OBJCOPY=avr-objcopy

CFLAGS=-DF_CPU=16000000UL

MMCU=atmega2560

ROOMBA_MAIN = main_roomba.c
CONTROLLER_MAIN = main_controller.c

# All source files EXCEPT main
LIB_SOURCES = \
	tta.c \
	sched_timer.c \
	uart/uart.c \
	roomba/roomba.c \
	message/message.c \
	joystick/joystick.c \
	button/button.c \
	photoresistor/photoresistor.c \
	servo/servo.c

SOURCES_CONTROLLER = $(LIB_SOURCES) $(CONTROLLER_MAIN)
SOURCES_ROOMBA = $(LIB_SOURCES) $(ROOMBA_MAIN)

all: $(BUILD)/$(CONTROLLER_OUT).hex $(BUILD)/$(ROOMBA_OUT).hex

upload_controller: $(BUILD)/$(CONTROLLER_OUT).hex
	$(Q)MMCU=$(MMCU) ./utils/program.py $<

upload_roomba: $(BUILD)/$(ROOMBA_OUT).hex
	$(Q)MMCU=$(MMCU) ./utils/program.py $<

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(Q)echo "COPY $@"
	$(Q)$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(BUILD)/$(CONTROLLER_OUT).elf: $(addprefix $(BUILD)/,$(SOURCES_CONTROLLER:.c=.o))
	$(Q)echo "CXX $@"
	$(Q)$(CC) $(CFLAGS) -mmcu=$(MMCU) -o $@ $^

$(BUILD)/$(ROOMBA_OUT).elf: $(addprefix $(BUILD)/,$(SOURCES_ROOMBA:.c=.o))
	$(Q)echo "CXX $@"
	$(Q)$(CC) $(CFLAGS) -mmcu=$(MMCU) -o $@ $^

$(BUILD)/%.o: %.c
	$(Q)echo "CC  $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -mmcu=$(MMCU) -c -o $@ $<

clean:
	rm -rf $(BUILD)
