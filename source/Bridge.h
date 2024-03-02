/*
	nvsonic Head Tracker OSC Bridge
	https://github.com/trsonic/nvsonic-head-tracker

	Copyright (c) 2017-2019 Tomasz Rudzki, Jacek Majer
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

#pragma once
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_osc/juce_osc.h>

#include "rs232.h"

class Bridge : private juce::Timer, private juce::OSCReceiver, private juce::OSCReceiver::Listener<juce::OSCReceiver::RealtimeCallback>
{
public:
    Bridge();
    ~Bridge() override;
    bool connectOscReceiver();
    void disconnectOscReceiver();

    void oscMessageReceived (const juce::OSCMessage& message) override;
    void oscBundleReceived (const juce::OSCBundle& bundle) override;

    juce::StringArray getPortInfo();
    bool connectSerial();
    void disconnectSerial();
    bool isSerialConnected();
    void timerCallback() override;
    void pushQuaternionVector();
    void resetOrientation();
    void updateEuler();

    float getRoll();
    float getPitch();
    float getYaw();
    float getRollOSC();
    float getPitchOSC();
    float getYawOSC();

    void setupQuatsOSC (bool isActive, juce::String address, juce::Array<int> order, juce::Array<int> signs);
    void setupRollOSC (bool isActive, juce::String address, float min, float max);
    void setupPitchOSC (bool isActive, juce::String address, float min, float max);
    void setupYawOSC (bool isActive, juce::String address, float min, float max);
    void setupRpyOSC (bool isActive, juce::String address, juce::String key);
    void setupIp (juce::String address, int port);

    int BaudR = 115200, PortN;
    int OSCinPortN = 53101;
    juce::String OSCinAddress = "/bridge/quat";

private:
    juce::StringPairArray portlist;

    int port_number, port_index, port_state;

    double qW = 1.0, qX = 0.0, qY = 0.0, qZ = 0.0;
    double qlW = 1.0, qlX = 0.0, qlY = 0.0, qlZ = 0.0;
    double qbW = 1.0, qbX = 0.0, qbY = 0.0, qbZ = 0.0;

    float m_roll = 0.0, m_pitch = 0.0, m_yaw = 0.0;
    bool m_quatsActive, m_rollActive, m_pitchActive, m_yawActive, m_rpyActive;
    juce::String m_quatsOscAddress;
    juce::Array<int> m_quatsOrder, m_quatsSigns;
    juce::String m_rollOscAddress, m_pitchOscAddress, m_yawOscAddress;
    float m_rollOscMin, m_pitchOscMin, m_yawOscMin;
    float m_rollOscMax, m_pitchOscMax, m_yawOscMax;
    juce::String m_rpyOscAddress, m_rpyOscKey;
    float m_rollOSC = 0.0, m_pitchOSC = 0.0, m_yawOSC = 0.0;

    bool m_serialPortConnected = false;
    juce::String m_ipAddress;
    int m_oscPortNumber;
    juce::OSCSender sender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Bridge)
};
