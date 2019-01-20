#include "DS1802.h"

DS1802::DS1802(int pCS, int pMute, int fClk)
{
	_pCS = pCS;
	pinMode(_pCS, OUTPUT);

	_pMute = pMute;
	pinMode(_pMute, OUTPUT);
	digitalWrite(_pMute, HIGH);

	disable();

	SPI.begin();
	SPI.setBitOrder(LSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(fClk);

	_v0 = DS1802_MIN;
	_v1 = DS1802_MIN;
	setMute(false, false);
}

void DS1802::toggleMute() {
	digitalWrite(_pMute, LOW);
	delay(50);
	digitalWrite(_pMute, HIGH);
#ifdef DBG
	Serial.println("DS1802::toggleMute");
#endif
}

void DS1802::setMuteMask(boolean m0, boolean m1) {
	_m0 = m0 ? 0xC0 : 0x80;
	_m1 = m1 ? 0xC0 : 0x80;
}

void DS1802::setMute(boolean m0, boolean m1) {
	setMuteMask(m0, m1);
	tx();
#ifdef DBG
	Serial.print("DS1802::setMute: ");
	Serial.print(m0 ? "1" : "0");
	Serial.print(",");
	Serial.println(m1 ? "1" : "0");
#endif
}

void DS1802::setValues(int v0, int v1) {
#ifdef DBG
	Serial.print("DS1802::setValues: ");
	Serial.print(v0);
	Serial.print(",");
	Serial.println(v1);
#endif
	setMuteMask(false, false);

	if (v0 < DS1802_MIN) v0 = DS1802_MIN;
	if (v0 > DS1802_MAX) v0 = DS1802_MAX;

	if (v1 < DS1802_MIN) v1 = DS1802_MIN;
	if (v1 > DS1802_MAX) v1 = DS1802_MAX;

	_v0 = v0;
	_v1 = v1;

	tx();
}

void DS1802::tx() {
	int vc0 = (_m0 | (0x3F & _v0));
	int vc1 = (_m1 | (0x3F & _v1));

	enable();
	SPI.transfer(vc0);
	SPI.transfer(vc1);
	disable();

#ifdef DBG
	Serial.print("DS1802::tx: ");
	Serial.print(vc0, BIN);
	Serial.print(" ");
	Serial.println(vc1, BIN);
#endif
}

void DS1802::enable() {
	digitalWrite(_pCS, HIGH);
}

void DS1802::disable() {
	digitalWrite(_pCS, LOW);
}


