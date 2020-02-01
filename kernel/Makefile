CFLAGS += -ffreestanding

include ../obj.mk

OBJS=arch/$(ARCH)/boot.o main.o

$(ROOT)/boot/kernel: $(OBJS) kernel.ld
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -T kernel.ld -o $@ $(OBJS) -lgcc

clean:
	rm $(OBJS)
