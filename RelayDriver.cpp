// 
// 
// 

#include "RelayDriver.h"

RelayDriver::RelayDriver()
{
	Init();
}

void RelayDriver::Init()
{
	pinMode(POWER_PIN, OUTPUT);
	int i = 0;
	for(i=0; i < INPUT_COUNT; i++)
		pinMode(intputList[i], OUTPUT);
	PowerOff();
	MuteAll();
	
}
void RelayDriver::PowerOn()
{
	digitalWrite(POWER_PIN, HIGH);
}

void RelayDriver::PowerOff()
{
	digitalWrite(POWER_PIN, LOW);
}

void RelayDriver::MuteAll()
{
	int i = 0;
	for (i = 0; i < INPUT_COUNT; i++)
		digitalWrite(intputList[i], LOW);
}

void RelayDriver::SetInput(int input)
{
	MuteAll();
	digitalWrite(input, HIGH);
}