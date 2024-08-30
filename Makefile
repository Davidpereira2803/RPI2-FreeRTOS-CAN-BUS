TOOLCHAIN ?= arm-none-eabi-

SOURCES = Demo/main.c \
          Demo/startup.c \
          Demo/Drivers/rpi_gpio.c \
          Demo/Drivers/rpi_irq.c \
          Demo/Tasks/ledcontrol.c \
          Demo/Tasks/binaryblink.c \
		  Demo/Tasks/time.c \
		  Demo/Tasks/Bench/gsm_enc/gsm_enc.c \
          Source/tasks.c \
          Source/list.c \
          Source/portable/GCC/RaspberryPi/port.c \
          Source/portable/GCC/RaspberryPi/portISR.c \
          Source/portable/GCC/RaspberryPi/portASM.c \
          Source/portable/MemMang/heap_4.c 

OBJECTS = $(patsubst %.c,build/%.o,$(SOURCES))

INCDIRS = Source/include Source/portable/GCC/RaspberryPi \
          Demo/Drivers Demo/

CFLAGS += -Wall $(addprefix -I ,$(INCDIRS))
CFLAGS += -I/usr/local/include
CFLAGS += -D RPI2
CFLAGS += -march=armv7-a -mtune=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4

ASFLAGS = -march=armv7-a -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard

LDFLAGS = -T Demo/raspberrypi.ld -Wl,-Map=kernel7.map -march=armv7-a -mtune=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4 -nostdlib -nostartfiles -lgcc

.PHONY: all clean

all: kernel7.img kernel7.list kernel7.syms kernel7.hex
	$(TOOLCHAIN)size kernel7.elf
	
build/%.o: %.c
	mkdir -p $(dir $@)
	$(TOOLCHAIN)gcc -c $(CFLAGS) $< -o $@
	
build/%.o: %.s
	mkdir -p $(dir $@)
	$(TOOLCHAIN)as $(ASFLAGS) $< -o $@

kernel7.elf: $(OBJECTS)
	$(TOOLCHAIN)gcc $(OBJECTS) -o $@ $(LDFLAGS)

kernel7.img: kernel7.elf
	$(TOOLCHAIN)objcopy kernel7.elf -O binary $@

kernel7.list: kernel7.elf
	$(TOOLCHAIN)objdump -D -S kernel7.elf > $@

kernel7.syms: kernel7.elf
	$(TOOLCHAIN)objdump -t kernel7.elf > $@

kernel7.hex: kernel7.elf
	$(TOOLCHAIN)objcopy kernel7.elf -O ihex $@

clean:
	rm -f $(OBJECTS)
	rm -f kernel7.list kernel7.img kernel7.syms
	rm -f kernel7.elf kernel7.hex kernel7.map 
	rm -rf build

