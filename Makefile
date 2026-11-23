.RECIPEPREFIX = >

DEVKITXENON ?= /usr/local/xenon
export PATH := $(DEVKITXENON)/bin:$(DEVKITXENON)/usr/bin:$(PATH)

TARGET  = grau360
CC      = xenon-gcc
OBJCOPY = xenon-objcopy

CFLAGS  = -O2 -Wall -mcpu=cell -mtune=cell -m32 -fno-pic -mpowerpc64 -DXENON -I$(DEVKITXENON)/usr/include
LDFLAGS = -n -T $(DEVKITXENON)/app.lds -L$(DEVKITXENON)/xenon/lib/32bit -L$(DEVKITXENON)/usr/lib
LIBS    = -lxenon -lm

all: $(TARGET).elf32

$(TARGET).elf: main.c
> $(CC) $(CFLAGS) main.c -o $@ $(LDFLAGS) $(LIBS)

$(TARGET).elf32: $(TARGET).elf
> $(OBJCOPY) -O elf32-powerpc --adjust-vma 0x80000000 $< $@

clean:
> rm -f $(TARGET).elf $(TARGET).elf32

.PHONY: all clean
