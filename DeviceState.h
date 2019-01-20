// DeviceState.h

#ifndef _DEVICESTATE_h
#define _DEVICESTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif


class DeviceState
{
public:
		int volume = -20;
		int inputNumber = 1;
		int power = 0;	
	
	    void(*VolumeCallback)() = NULL;
		void(*PowerCallback)() = NULL;
		void(*InputCallback)() = NULL;
		void SetVolume(int volume);
		void IncreaseVolume();
		void DecreaseVolume();
		void SetInputNumber(int inputNumber);
		void SetPower(int power);
};
