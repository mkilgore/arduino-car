
TARGET := arduino-car

MCU := atmega328p
F_CPU := 16000000UL
BAUD := 38400UL
AVRDUDE_MCU := atmega328p

PROGRAMMER_TYPE := arduino
PROGRAMMER_ARGS := -P /dev/ttyACM0 -b 115200

PREFIX := avr-
CC := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
AVRSIZE := avr-size
AVRDUDE := avrdude

SRCS := $(wildcard ./src/*.c)
OBJS := $(SRCS:.c=.o)

CPPFLAGS := -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I./include

CFLAGS += -Os -g -std=gnu99 -Wall
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fwrapv

CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS := -wl,-Map,$(TARGET).map
LDFLAGS := -Wl,--gc-sections

TARGET_ARCH := -mmcu=$(MCU)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

.PHONY: all eeprom clean flash flash_eeprom fuses show_fuses

clean:
	rm -f $(OBJS)
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).eeprom

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(AVRDUDE_MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flash_eeprom: $(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(AVRDUDE_MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

