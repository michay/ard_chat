// Do not remove the include below
#include "hardware.h"
#include "HardwareSerial.h"

#include "process/process.h"
#include "commands/commands.h"

extern HardwareSerial Serial;

#define LED_PIN 13

static unsigned long last_read;

//The setup function is called once at startup of the sketch
void setup()
{
	pinMode(LED_PIN, OUTPUT);
	last_read = millis();

	Serial.begin(9600);
}

// The loop function is called in an endless loop
void loop()
{
	unsigned long now = millis();

	if(Serial.available())
	{
		unsigned char incomingByte = Serial.read();
		EProcessRX status = process_char(incomingByte);
		if(status == EProcessRX_success)
		{
			Serial.write("\r\nSuccess - ");
			Serial.write(State.sMessage.data);
		}
		else if(status == EProcessRX_invald_checksum)
		{
			Serial.write(State.sMessage.data);
		}
		else if (status == EProcessRX_no_SOM_received)
		{
			Serial.write(incomingByte);
		}

		if(incomingByte == '\r')
			Serial.write("\nchatme> ");

		// light led to show process
		last_read = millis();
		digitalWrite(LED_PIN, HIGH);
	}

	// hide led after a second
	if(now - last_read > 1000)
		digitalWrite(LED_PIN, LOW);
}
