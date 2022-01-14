#ifndef _COMMUNICATION_SCHEME_
#define _COMMUNICATION_SCHEME_

#define DATA_TRANSFER_IDENTIFIER_EXAMPLE (0xAB)
#define B_1_STRING_TRANSFER_IDENTIFIER (0xB1)
#define B_1_STRING_ADDRESS (0x500)
#define B_2_EXIT_COUNT_IDENTIFIER (0xB2)

#define C_1_PATH_ADDRESS (0x700)
#define C_1_OPEN_PATH_TRANSFER_IDENTIFIER (0x0A)

#define C_3_WRITE_FILE_TRANSFER_IDENTIFIER (0xDC)

/* 
    The read struct is:
    int len; (input target length to read and)
    char *buffer;

    And eax - output length that was actually read
*/
#define C_2_READ_BUFFER_STRUCT_ADDRESS (0x750)
#define C_2_READ_FILE_TRANSFER_IDENTIFIER (0xCD)

#define C_4_CLOSE_FILE_TRANSFER_IDENTIFIER (0xCF)

#endif /* _COMMUNICATION_SCHEME_ */