# Arduino library for reading from the LTC2440 ADC

## Weird caveats
Haven't figured how to handle the data ready flag on the LTC2440 yet so don't sample more than the programmed sampling rate. Additionally, I haven't implemented programmable sampling rates so you're limited to 880Hz by setting SDI to GND or 7Hz by setting SDI to VCC.

The same sampling rate restriction applies to the .Tare(..) function, ie, pass >=2 into the millis_delay parameter.

## Wiring
I'm using the DC570A evaluation board.

| DC 570A   | Teensy  | 
| :------------- | :----------: | 
| SDO | MISO   | 
| SDI   | GND (sets the sampling rate to 880Hz)| 
| CS | pin 10 | 
| VCC | 3v3|
| GND | GND|

## Example 
Written for Platformio.
```cpp
#include <Arduino.h>
#include "ltc2440.h"

#define kLTC2440CSPin 10

int32_t last_reading = 0;
float dt = 0.002; // [s]
float cutoff_dt = 0.005; // [s]
int32_t reading = 0;
int32_t last_print = 0;
float print_dt = 0.01; //[s]


LTC2440 torque_sensor(kLTC2440CSPin);

void setup() {
  Serial.begin(115200);
  torque_sensor.Init();

  delay(1000);
  torque_sensor.Tare(1000, 2);
}

void loop() {
  reading = torque_sensor.Read();
  float alpha = dt / (cutoff_dt + dt);
  last_reading = reading * alpha + last_reading * (1-alpha);
  delayMicroseconds(1000000 * dt);

  if (millis() - last_print > 1000 * print_dt) {
    last_print = millis();
    Serial.println(last_reading);
  }
}
```