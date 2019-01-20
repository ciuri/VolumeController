
#include "DeviceState.h"
#include "RelayDriver.h"
#include "pga2311.h"
#include "RC5.h"
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <SPI.h>


#define encoder0PinA  2
#define encoder0PinB  3

volatile unsigned int encoder0Pos = 0;

unsigned long lastDisplayModeChangeMiliseconds = 0;
unsigned long lastPowerOffCommandMiliseconds = 0;
int tryToOffCount = 0;
int displayMode = 0;

#define ENCODER_PIN_SWITCH 4

U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0);

PGA2311 pga2310(9, 7);
RC5 rc5(8);
RelayDriver relayDriver;
int currentEncoderPos;
DeviceState deviceState;
const uint8_t *FONT = u8g2_font_crox5h_tf;

void setup()
{
	deviceState.VolumeCallback = OnVolumeChange;
	deviceState.PowerCallback = OnPowerChange;
	deviceState.InputCallback = OnInputChange;

	pinMode(encoder0PinA, INPUT);
	pinMode(encoder0PinB, INPUT);

	// encoder pin on interrupt 0 (pin 2)
	attachInterrupt(0, doEncoderA, CHANGE);

	// encoder pin on interrupt 1 (pin 3)
	attachInterrupt(1, doEncoderB, CHANGE);
	pinMode(ENCODER_PIN_SWITCH, INPUT_PULLUP);
	
	Serial.begin(115200);
	display.begin();
	Serial.write("Device START");
	pga2310.init();
	pga2310.unmuteAll();
	currentEncoderPos = encoder0Pos;
	//relayDriver.SetInput(INPUT1);
	OnPowerChange();
}

void loop()
{	
	CheckEncoderAndSwitch();
	CheckRC5();
	CheckMiliseconds();
}

void CheckMiliseconds()
{
	if (displayMode == 1 && millis() - lastDisplayModeChangeMiliseconds > 3000)
	{
		displayMode = 0;
		Serial.write("display mode = 0");
		DrawDisplay(&deviceState);
	}
}

void CheckEncoderAndSwitch()
{
	if (digitalRead(ENCODER_PIN_SWITCH) == 0)
	{
		delay(200);
		if (deviceState.power == 0)
		{
			deviceState.SetPower(1);
			return;
		}
		if (deviceState.power == 1)
		{
			if (lastDisplayModeChangeMiliseconds == 0)			
				lastPowerOffCommandMiliseconds = millis();	
			delay(200);
			Serial.write("try to off\n");
			tryToOffCount++;

			if (encoder0Pos > currentEncoderPos)
			{
				currentEncoderPos = encoder0Pos;
				deviceState.SetInputNumber(++deviceState.inputNumber);
				tryToOffCount = 0;
				delay(200);
				currentEncoderPos = encoder0Pos;
				return;
			}
			if (encoder0Pos < currentEncoderPos)
			{				
				currentEncoderPos = encoder0Pos;
				deviceState.SetInputNumber(--deviceState.inputNumber);
				tryToOffCount = 0;
				delay(200);
				currentEncoderPos = encoder0Pos;
				return;
			}


			if (tryToOffCount == 6)
			{
				deviceState.SetPower(0);
				delay(1000);
			}
			
		}
	}
	else
	{
		tryToOffCount = 0;
	}
	


	if (encoder0Pos > currentEncoderPos)
	{
		currentEncoderPos = encoder0Pos;
		deviceState.IncreaseVolume();
		return;
	}
	if (encoder0Pos < currentEncoderPos)
	{
		deviceState.DecreaseVolume();
		currentEncoderPos = encoder0Pos;
		return;
	}
}

void CheckRC5()
{
	unsigned char toggle;
	unsigned char address;
	unsigned char command;
	if (rc5.read(&toggle, &address, &command))
	{
		char debugText[32];
		sprintf(debugText, "Command: %i, Address: %i, Toggle: %i\n", command, address, toggle);

		Serial.print(debugText);
		
		if (command == 16 && address==16 && deviceState.volume < 0)
			deviceState.IncreaseVolume();
		if (command == 17 && address == 16 && deviceState.volume > -95)
			deviceState.DecreaseVolume();
		if (command == 2 && address == 16)
			deviceState.SetInputNumber(1);
		if (command == 5 && address == 16)
			deviceState.SetInputNumber(2);
		if (command == 3 && address == 16)
			deviceState.SetInputNumber(3);
		if (command == 1 && address == 16)
			deviceState.SetInputNumber(4);
		if (command == 13 && address == 16)
			deviceState.SetPower(0);		
	}
}

void OnVolumeChange()
{
	displayMode = 1;
	lastDisplayModeChangeMiliseconds = millis();
	pga2310.setGain((float)deviceState.volume, (float)deviceState.volume);
	DrawDisplay(&deviceState);
}

void OnPowerChange()
{
	if (deviceState.power == 0)
	{
		pga2310.muteAll();
		relayDriver.MuteAll();
		relayDriver.PowerOff();
	}
	if (deviceState.power == 1)
	{
		relayDriver.MuteAll();
		relayDriver.PowerOn();
		OnInputChange();
		pga2310.setGain((float)deviceState.volume, (float)deviceState.volume); // todo should be OnVolumeChange but... 
	}
	DrawDisplay(&deviceState);
}

void OnInputChange()
{
	displayMode = 0;
	pga2310.muteAll();
	switch (deviceState.inputNumber)
	{
	case 1:
		relayDriver.SetInput(INPUT1);
		break;
	case 2:
		relayDriver.SetInput(INPUT2);
		break;
	case 3:
		relayDriver.SetInput(INPUT3);
		break;
	case 4:
		relayDriver.SetInput(INPUT4);
		break;
	}
	pga2310.unmuteAll();
	DrawDisplay(&deviceState);
}

void DrawDisplay(DeviceState* state)
{
	display.firstPage();
	do
	{
		if (displayMode == 1)
		{
			display.setFont(FONT);
			char text1[10];
			char text2[10];
			sprintf(text1, "%idB", (int)state->volume);		
			display.drawStr(30, 40, text1);
		
		}
		else
		{
			if (state->power == 1)
			{
				display.setFont(FONT);
				char text1[10];
				char text2[10];
				sprintf(text1, "%idB", (int)state->volume);
				sprintf(text2, "Input: %i", state->inputNumber);
				display.drawStr(30, 20, text1);
				//display.drawLine(1, 32, 128, 32);
				display.drawStr(25, 54, text2);
			}
			else
			{
				display.setFont(FONT);
				char text1[10];
				char text2[10];
				sprintf(text1, "Standby");
				//sprintf(text2, "Input %i", state->inputNumber);
				display.drawStr(20, 30, text1);
				//display.drawLine(1, 32, 128, 32);
				//display.drawStr(10, 54, text2);

			}
		}
	} while (display.nextPage());

}


void doEncoderA() {
	// look for a low-to-high on channel A
	if (digitalRead(encoder0PinA) == HIGH) {

		// check channel B to see which way encoder is turning
		if (digitalRead(encoder0PinB) == LOW) {
			encoder0Pos = encoder0Pos + 1;         // CW
		}
		else {
			encoder0Pos = encoder0Pos - 1;         // CCW
		}
	}

	else   // must be a high-to-low edge on channel A
	{
		// check channel B to see which way encoder is turning
		if (digitalRead(encoder0PinB) == HIGH) {
			encoder0Pos = encoder0Pos + 1;          // CW
		}
		else {
			encoder0Pos = encoder0Pos - 1;          // CCW
		}
	}
	Serial.println(encoder0Pos, DEC);
	// use for debugging - remember to comment out
}

void doEncoderB() {
	// look for a low-to-high on channel B
	if (digitalRead(encoder0PinB) == HIGH) {

		// check channel A to see which way encoder is turning
		if (digitalRead(encoder0PinA) == HIGH) {
			encoder0Pos = encoder0Pos + 1;         // CW
		}
		else {
			encoder0Pos = encoder0Pos - 1;         // CCW
		}
	}

	// Look for a high-to-low on channel B

	else {
		// check channel B to see which way encoder is turning
		if (digitalRead(encoder0PinA) == LOW) {
			encoder0Pos = encoder0Pos + 1;          // CW
		}
		else {
			encoder0Pos = encoder0Pos - 1;          // CCW
		}
	}
}