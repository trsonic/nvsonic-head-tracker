# nvsonic 3DOF Head Tracker
![alt text](https://github.com/trsonic/head-tracker/blob/master/images/ht_white_bg.jpg "")

## nvsonic 3DOF Head Tracker is a low-cost solution for spatial audio monitoring using headphones
Head tracking is a key element of the headphone based spatial audio. Virtual Reality headsets like Rift, Vive and VR ready mobile phones use built-in inertial sensors to track user’s head movements. For those who would like to experience spatial sound without wearing a VR headset, we have created a DIY head tracker that can be attached to the user’s headphones. With nvsonic 3DOF Head Tracker, you can monitor audio produced with spatial audio plugins, e.g, [ambiX](http://www.matthiaskronlachner.com/?p=2015), [IEM Plug-in Suite](https://plugins.iem.at/), [Facebook 360 Spatial Workstation](https://facebook360.fb.com/spatial-workstation/). The total cost of the components is around $20. After assembling the device you can use our interfacing software to control plugins within your DAW.

## Project Authors
- [Tomasz Rudzki](https://github.com/trsonic/) - PhD Student at [AudioLab](https://audiolab.york.ac.uk/), University of York, UK
- Jacek Majer - PhD Student at Warsaw University of Technology, Poland

The IMU code is based on: https://github.com/rpicopter/ArduinoMotionSensorExample

## Required Components
1. MPU-9250 or MPU-9150 orientation sensor
2. Arduino Pro Micro board
3. Gold pin connectors, wires and Micro USB cable

![alt text](https://github.com/trsonic/head-tracker/blob/master/images/mpu9250_9150.png "")
![alt text](https://github.com/trsonic/head-tracker/blob/master/images/arduino_promicro.png "")
![alt text](https://github.com/trsonic/head-tracker/blob/master/images/gold_pin_connector.jpg "")
![alt text](https://github.com/trsonic/head-tracker/blob/master/images/gold_pin_wires.png "")
![alt text](https://github.com/trsonic/head-tracker/blob/master/images/micro_usb_cable.jpg "")

You can source necessary components from eBay:
- [MPU-9250](http://www.ebay.com/sch/?_nkw=MPU-9250)
- [MPU-9150](http://www.ebay.com/sch/?_nkw=MPU-9150)
- [Arduino Pro Micro with ATmega32U4 microcontroller](http://www.ebay.com/sch/i.html?_nkw=Arduino+Pro+Micro+ATmega32U4)

## Assembling the Head Tracker
Connect the IMU sensor with Arduino board according to the schematic presented below.

![alt text](https://github.com/trsonic/head-tracker/blob/master/images/schematic.png "")

## Flashing Arduino
- Connect the assembled Head Tracker to your computer using Micro USB cable.
- Clone / download this repository.
- Open the Arduino [sketch](https://github.com/trsonic/nvsonic-head-tracker/blob/master/head-tracker-arduino/head-tracker-arduino.ino) in [Arduino IDE](https://www.arduino.cc/en/Main/Software). Choose 'Leonardo Arduino Board' in Tools menu as well as the appropriate COM port.
- Compile and upload the sketch to your device.

## Attaching to Your Headphones
Mount the sensor on the top of the headband, centered. The wires should be oriented towards the left ear side. The Arduino board can be attached to the side of the headband. You can use zip ties or velcro straps to fix the elements. Use your creativity:)

![alt text](https://github.com/trsonic/head-tracker/blob/master/images/sensor_headband.png "")

## Interfacing Software
- Download the [latest release](https://github.com/trsonic/nvsonic-head-tracker/releases) of the Head Tracker OSC Bridge or compile it yourself.
- Run *Head Tracker OSC Bridge.exe / .app*. The current release was designed to work with Reaper and Matthias Kronlachner's Ambisonic rotation ([ambix_rotator](http://www.matthiaskronlachner.com/?p=2015)) plugin. Set the Euler rotation order inside the plugin to: roll-pitch-yaw.

![alt text](https://github.com/trsonic/head-tracker/blob/master/images/ambix_rotator.png "")

For assigning the OSC messages to plugin parameters use “learn” DAW functionality. Toggle “M” (mute) buttons to control which parameter is being sent. Sending one type of parameter (Roll, Pitch or Yaw) at a time is required for this process.

![alt text](https://github.com/trsonic/head-tracker/blob/master/images/app_screen_shot.png "")

## Drift Performance
You can experience some drift during the first few minutes of operation. Stay calm, the sensor needs to stabilize its temperature to provide accurate direction in horizontal plane. 

## Head Tracking in Reaper
To remove the nasty latency caused by Reaper's audio processing, turn off the anticipative FX processing in preferences.

![alt text](https://github.com/trsonic/head-tracker/blob/master/images/reaper_fx_processing.png "")



