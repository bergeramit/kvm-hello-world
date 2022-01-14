#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "communication_scheme.h"

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

void place_string_in_address(const char *str, char *address_pointer) {
	char i = 0;
	while(*(str + i) != 0) {
		*(char *) (address_pointer + i) = *(str + i);
		i++;
	}
	*(char *) (address_pointer + i) = '\0';
}

void print(const char *str) {
	place_string_in_address(str, (char *)B_1_STRING_ADDRESS);
	data_out_to_hypervisor(B_1_STRING_TRANSFER_IDENTIFIER, B_1_STRING_ADDRESS);
	return;
}

int exits(void) {
	return data_input_from_hypervisor(B_2_EXIT_COUNT_IDENTIFIER);
}

int open(const char *path) {
	place_string_in_address(path, (char *)C_1_PATH_ADDRESS);
	data_out_to_hypervisor(C_1_OPEN_PATH_TRANSFER_IDENTIFIER, C_1_PATH_ADDRESS);
	return 0;
}

int read(void *buf, int len) {
	* (int *) buf = len;
	data_out_to_hypervisor(C_2_READ_FILE_TRANSFER_IDENTIFIER, C_2_READ_BUFFER_STRUCT_ADDRESS);
	return 0;
}

void close() {
	data_out_to_hypervisor(C_4_CLOSE_FILE_TRANSFER_IDENTIFIER, 0);
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	char exit_print_format[12] = "X exits\n";
	uint32_t exit_count_before_test = 0, exit_count_after = 0;
	uint32_t from_hypervisor;

	for (p = "Hello, world!\n"; *p; ++p)
		outb(0xE9, *p);

	data_out_to_hypervisor(DATA_TRANSFER_IDENTIFIER_EXAMPLE, 0x1337);
	from_hypervisor = data_input_from_hypervisor(DATA_TRANSFER_IDENTIFIER_EXAMPLE);
	data_out_to_hypervisor(DATA_TRANSFER_IDENTIFIER_EXAMPLE, from_hypervisor);

	print("(b.1) This is a user string");
	print("(b.1) Successful run!\n");

	print("(b.2) hypercalls to 'exits' also count as exits :)");
	print("(b.2) output of the guest after a single print should be 2\n\t\tone for the print and one for the exits hypercall");
	print("(b.2) calling exits...");
	exit_count_before_test = exits();
	print("Sample Print to change the exit count,\n\t\tnow calling exits again...");
	exit_count_after = exits();
	exit_print_format[0] = (char)(exit_count_after - exit_count_before_test) + '0';
	print(exit_print_format);

	print("(C.1)");
	open("temp.txt");
	read((void *)C_2_READ_BUFFER_STRUCT_ADDRESS, 2);

	*(long *) 0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
