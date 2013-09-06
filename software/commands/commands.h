#pragma once

#define MAX_MESSAGE_SIZE 30
#define PC_ARD_SOM 0xF0
#define ARD_PC_SOM 0xF1

void create_message_to_ard(unsigned char* data, unsigned char length);

extern unsigned char tx_pc_ard_buffer[];
extern int tx_pc_ard_length;
