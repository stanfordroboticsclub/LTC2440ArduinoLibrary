/*
 * ltc2440.h
 *
 *  Created on: Oct 2020
 *      Author: Nathan Kau, heavily based on code by Lukas Martisiunas
 *      Description: Basic library to initialise and use LTC2440 with Arduino
 */
#ifndef LTC2440_H
#define LTC2440_H

#include "Arduino.h"

class LTC2440 {
 private:
  uint8_t chip_select_pin_;
  int32_t adc_offset_ = 0;

 public:
  LTC2440(uint8_t chip_select_pin);
  int32_t Read();
  void Tare(int samples, int millis_delay);
  void Init();
};

#endif
