#include "rs232_interface.h"
#include "../rs232/rs232.h"

int  rs232_comm_connect   (void)
{
	return RS232_OpenComport(COMPORT, COMBAUDRATE);
}
void rs232_comm_disconnect(void)
{
	RS232_CloseComport(COMPORT);
}

int rs232_comm_read_data (unsigned char* buffer, int max_buffer_size)
{
	return RS232_PollComport(COMPORT, buffer, max_buffer_size);
}

int rs232_comm_send_data (unsigned char* buffer, int max_buffer_size)
{
	return RS232_SendBuf(COMPORT, buffer, max_buffer_size);
}
