#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "../communication/comm_thread.h"
#include "../communication/comm_defenitions.h"
#include "../process/process.h"


EProcessRX handle_received_byte (sCommData* comm, unsigned char rx);

void create_command(ECommandCodes opcode, char* data, int len);
void create_nick_command(EGetSet getset, char* new_nick, int len);

#endif
