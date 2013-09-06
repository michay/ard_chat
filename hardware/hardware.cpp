// Do not remove the include below
#include "hardware.h"
#include "HardwareSerial.h"

#include "process/process.h"
#include "commands/commands.h"
#include "utils/error_codes.h"
#include "utils/buffers.h"

#include <string.h>

extern HardwareSerial Serial;

#define LED_PIN 13

static unsigned long last_read;

// Terminal interface
void write_to_terminal(char* str);
void report_error(char* str, EErrorCodes error_code);

//
// buffers
//
static unsigned char buff_ard_pc_buffer[ARD_PC_BUFFER_MAX_SIZE];

//The setup function is called once at startup of the sketch
void setup()
{
	// Set pin 13 as terminal indication led
	pinMode(LED_PIN, OUTPUT);
	last_read = millis();

	// Start serial communication
	Serial.begin(9600);

	// Init state
	State.eRxState = ERxState_SOM;
	strcpy(State.nick, "newbie\0");

	// Setup buffers
	Buffers.ard_pc_buffer.buff = buff_ard_pc_buffer;
	Buffers.ard_pc_buffer.buff_size = 0;

	// Setup terminal interface
	TerminalInterface.write_to_terminal = write_to_terminal;
	TerminalInterface.report_error = report_error;
}

// The loop function is called in an endless loop
void loop()
{
	unsigned long now = millis();

	// Check if anyting received from serial port
	if(Serial.available())
	{
		// handle terminal communication
		handle_terminal_comm(Serial.read());
	}

	// Check if anything to send to serial port
	if(Buffers.ard_pc_buffer.buff_size > 0)
	{
		// Send data
		Serial.write(Buffers.ard_pc_buffer.buff, Buffers.ard_pc_buffer.buff_size);
		Buffers.ard_pc_buffer.buff_size = 0;

		// light led to show process
		last_read = millis();
		digitalWrite(LED_PIN, HIGH);
}

	// hide led after a second
	if(now - last_read > 1000)
		digitalWrite(LED_PIN, LOW);
}


// Terminal interface
void write_to_terminal(char* str)
{
	Serial.write(str);
}

void report_error(char* str, EErrorCodes error_code)
{
	Serial.write("\r\n");
	Serial.write(str);
	Serial.print((int)error_code);
}
