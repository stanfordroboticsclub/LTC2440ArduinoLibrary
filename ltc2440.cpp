/*
 * ltc2440.c

 *
 *  Created on: 7 Jul 2017
 *      Author: Lukas Martisiunas
 *
 */
#include "ltc2440.h"

#include "Arduino.h"
#include "SPI.h"

int32_t SpiRead(uint8_t slave_cs_pin);

int32_t SpiRead(uint8_t slave_cs_pin) {
  int32_t result = 0;
  int32_t b;

  SPI.beginTransaction(SPISettings(3000000, MSBFIRST, SPI_MODE0));
  digitalWrite(slave_cs_pin, LOW);  // take the SS pin low to select the chip
  delayMicroseconds(10);         // probably not needed, only need 25 nsec delay

  byte sign = 0;
  b = SPI.transfer(0xff);  // B3
  if (b >> 4 == 0b0010 || b >> 4 == 0b0001) {
    if ((b & 0x20) == 0) {
      sign = 1;
    }
    b &= 0x1f;
    result = b;
    result = result << 8;
    b = SPI.transfer(0xff);  // B2
    result |= b;
    result = result << 8;
    b = SPI.transfer(0xff);  // B1
    result |= b;
    result = result << 8;
    b = SPI.transfer(0xff);  // B0
    result |= b;
    // take the SS pin high to de-select the chip:
    digitalWrite(slave_cs_pin, HIGH);
    SPI.endTransaction();

    if (sign) result |= 0xf0000000;
    return (result);
  } else {
    digitalWrite(slave_cs_pin, HIGH);
    SPI.endTransaction();
    Serial.print("First 8 bits (first 4 should be 0010 for + or 0001 for -): ");
    Serial.println(b, BIN);
    return (0);
  }
}

LTC2440::LTC2440(uint8_t chip_select_pin) : chip_select_pin_(chip_select_pin) {}

int32_t LTC2440::Read() {
  /*
  Don't call faster than the sampling LTC2440 sampling rate because I didn't
  implement the thing that checks for ready data
  */
  int32_t read_adc_val = SpiRead(chip_select_pin_);
  read_adc_val = read_adc_val >> 5;  // truncate lowest 5 bits
  return read_adc_val - adc_offset_;
}

void LTC2440::Init() {
  pinMode(chip_select_pin_, OUTPUT);
  // chip select is active low
  digitalWrite(chip_select_pin_, HIGH);
  SPI.begin();

  // vestiges of an old library - not sure if needed
  char temp_2 = SpiRead(chip_select_pin_);
  temp_2 = SpiRead(chip_select_pin_);
}

void LTC2440::Tare(int samples, int millis_delay) {
  int32_t average_reading = 0;
  for (int i = 0; i < samples; i++) {
    average_reading += Read() / float(samples);
    delay(millis_delay);
  }
  adc_offset_ = average_reading;
}
