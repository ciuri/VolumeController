// RelayDriver.h

#ifndef _RELAYDRIVER_h
#define _RELAYDRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define INPUT1 6
#define INPUT2 17
#define INPUT3 16
#define INPUT4 15
#define POWER_PIN 5
#define INPUT_COUNT 4

class RelayDriver
{
public:
	RelayDriver();
	void SetInput(int input);
	void PowerOn();
	void PowerOff();
	void MuteAll();

private:
	int intputList[INPUT_COUNT] = {INPUT1, INPUT2, INPUT3, INPUT4};
	void Init();
	
	
};



#endif

