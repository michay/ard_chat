#pragma once

#define COMPORT 3
#define COMBAUDRATE 9600

int  rs232_comm_connect   (void);
void rs232_comm_disconnect(void);
int  rs232_comm_read_data (unsigned char* buffer, int max_buffer_size);
int  rs232_comm_send_data (unsigned char* buffer, int max_buffer_size);
