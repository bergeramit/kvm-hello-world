#include <stddef.h>
#include <stdint.h>

#define DATA_TRANSFER_IDENTIFIER (0xAB)

static void outb(uint16_t port, uint8_t value) {
	asm("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

static inline void data_out_to_hypervisor(uint16_t data_transfer_indentifier, uint32_t value) {
	asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (data_transfer_indentifier) : "memory");
}

static inline uint32_t data_input_from_hypervisor(uint16_t data_transfer_indentifier) {
	uint32_t ret;
	asm("in %1, %0" : "=a" (ret) : "Nd" (data_transfer_indentifier) : "memory" );
	return ret;
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	uint32_t from_hypervisor;

	for (p = "Hello, world!\n"; *p; ++p)
		outb(0xE9, *p);

	data_out_to_hypervisor(DATA_TRANSFER_IDENTIFIER, 0x1337);
	from_hypervisor = data_input_from_hypervisor(DATA_TRANSFER_IDENTIFIER);
	data_out_to_hypervisor(DATA_TRANSFER_IDENTIFIER, from_hypervisor);

	*(long *) 0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
