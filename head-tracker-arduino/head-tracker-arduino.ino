/*
  nvsonic Head Tracker
  https://github.com/trsonic/nvsonic-head-tracker

  Copyright (c) 2017-2019 Tomasz Rudzki
  Email: tom@nvsonic.io
  Website: https://nvsonic.io/
  Twitter: @tomasz_rudzki

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* This code is based on: https://github.com/rpicopter/ArduinoMotionSensorExample
and was customized by Tomasz Rudzki */

#include "freeram.h"
#include "mpu.h"
#include "I2Cdev.h"

#define DISPLAY_INTERVAL  20

void setup() {
    Fastwire::setup(400,0);
    Serial.begin(115200);
    mympu_open(200);
}

unsigned long lastDisplay = 0;

void loop() {
    unsigned long now = millis();
    mympu_update();

    if ((now - lastDisplay) >= DISPLAY_INTERVAL)
    {
      char imu_data[64];
      char qW[8], qX[8], qY[8], qZ[8];
      
      dtostrf(mympu.qW, 7, 4, qW);
      dtostrf(mympu.qX, 7, 4, qX);
      dtostrf(mympu.qY, 7, 4, qY);
      dtostrf(mympu.qZ, 7, 4, qZ);   
      
      strcpy(imu_data,qW);
      strcat(imu_data,",");
      strcat(imu_data,qX);
      strcat(imu_data,",");
      strcat(imu_data,qY);
      strcat(imu_data,",");
      strcat(imu_data,qZ);

      Serial.write(imu_data);
      Serial.write(";");
      lastDisplay = now;
    }    
}
