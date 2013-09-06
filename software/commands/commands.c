#include "commands.h"

#include <string.h>

unsigned char tx_pc_ard_buffer[MAX_MESSAGE_SIZE];
int tx_pc_ard_length;


void create_message_to_ard(unsigned char* data, unsigned char length)
{
	tx_pc_ard_buffer[0] = PC_ARD_SOM;
	tx_pc_ard_buffer[1] = length - 1;

	// skip the '/' sign
	memcpy(tx_pc_ard_buffer + 2, data + 1, length - 1);

	unsigned char running_checksum = 0;
	tx_pc_ard_length = 2 + (length - 1);
	for(int i = 0; i < tx_pc_ard_length; ++i)
		running_checksum += tx_pc_ard_buffer[i];

	// set checksum
	tx_pc_ard_buffer[tx_pc_ard_length++] = ~running_checksum + 1;
}
