#include <stddef.h>
#include <stdint.h>

#define DATA_TRANSFER_IDENTIFIER_EXAMPLE (0xAB)
#define C_1_STRING_TRANSFER_IDENTIFIER (0xEE)
#define C_1_STRING_ADDRESS (0x500)

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

void print(const char *str) {
	char i = 0;
	char *address_pointer = (char *)C_1_STRING_ADDRESS;

	while(*(str + i) != 0) {
		*(char *) (address_pointer + i) = *(str + i);
		i++;
	}
	*(char *) (address_pointer + i) = '\0';

	data_out_to_hypervisor(C_1_STRING_TRANSFER_IDENTIFIER, C_1_STRING_ADDRESS);
	return;
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	uint32_t from_hypervisor;

	for (p = "Hello, world!\n"; *p; ++p)
		outb(0xE9, *p);

	data_out_to_hypervisor(DATA_TRANSFER_IDENTIFIER_EXAMPLE, 0x1337);
	from_hypervisor = data_input_from_hypervisor(DATA_TRANSFER_IDENTIFIER_EXAMPLE);
	data_out_to_hypervisor(DATA_TRANSFER_IDENTIFIER_EXAMPLE, from_hypervisor);

	print("--(b.1)--\n");
	print("(b.1) Another One!\n");

	*(long *) 0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
