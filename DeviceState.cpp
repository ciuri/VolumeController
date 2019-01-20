// 
// 
// 

#include "DeviceState.h"

void DeviceState::SetInputNumber(int in)
{
	if (power == 0)
	{
		SetPower(1);
		return;
	}

	if (in < 1 || in >4)
		return;
	inputNumber = in;
	if (InputCallback != NULL)
		InputCallback();
}

void DeviceState::SetVolume(int vol)
{
	if (power == 0)
		return;

	volume = vol;
	if (VolumeCallback != NULL)
		VolumeCallback();
}

void DeviceState::SetPower(int p)
{
	power = p;
	if (PowerCallback != NULL)
		PowerCallback();
}

void DeviceState::IncreaseVolume()
{
	if (power == 0)
		return;

	if (volume < 0)
	{
		volume++;
		if (VolumeCallback != NULL)
			VolumeCallback();
	}
}

void DeviceState::DecreaseVolume()
{
	if (power == 0)
		return;
	volume--;
	if (VolumeCallback != NULL)
		VolumeCallback();
}