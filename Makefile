# PIC32 device number
DEVICE		= 32MX320F128H

# UART settings for programmer
TTYDEV		?=/dev/ttyUSB0
TTYBAUD		?=115200

# Name of the project
PROGNAME	= outfile

# Compiler and linker flags
CFLAGS		+= -Wall -std=c99 -ffreestanding -march=mips32r2 -msoft-float -Wa,-msoft-float -G 0
ASFLAGS		+= -msoft-float
LDFLAGS		+= -T "p$(shell echo "$(DEVICE)" | tr '[:upper:]' '[:lower:]').ld"

# Filenames
ELFFILE		= $(PROGNAME).elf
HEXFILE		= $(PROGNAME).hex

# Find all source files automatically
CFILES          = $(wildcard *.c)
ASFILES         = $(wildcard *.S)

# Object file names
OBJFILES        = $(CFILES:.c=.c.o)
OBJFILES        +=$(ASFILES:.S=.S.o)

.PHONY: all clean install envcheck

all: $(HEXFILE)

clean:
	$(RM) $(HEXFILE) $(ELFFILE) $(OBJFILES)

envcheck:
	@echo "$(TARGET)" | grep pic32 > /dev/null || (\
		echo ""; \
		echo " **************************************************************"; \
		echo " * Make sure you have sourced the cross compiling environment *"; \
		echo " * Do this by issuing:                                        *"; \
		echo " * . /path/to/crosscompiler/environment                       *"; \
		echo " **************************************************************"; \
		echo ""; \
		exit 1)

install: envcheck
	$(TARGET)avrdude -v -p $(shell echo "$(DEVICE)" | tr '[:lower:]' '[:upper:]') -c stk500v2 -P "$(TTYDEV)" -b $(TTYBAUD) -U "flash:w:$(HEXFILE)"

$(ELFFILE): $(OBJFILES) envcheck
	$(CC) $(CFLAGS) -o $@ $(OBJFILES) $(LDFLAGS)

$(HEXFILE): $(ELFFILE) envcheck
	$(TARGET)bin2hex -a $(ELFFILE)

%.c.o: %.c envcheck
	$(CC) $(CFLAGS) -c -o $@ $<

%.S.o: %.S envcheck
	$(CC) $(CFLAGS) -c -o $@ $<
