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

#include "../JuceLibraryCode/JuceHeader.h"
#include "rs232.h"

class Bridge : private Timer

{
public:
    Bridge();    
    ~Bridge();
    
	StringArray getPortInfo();
    bool connectBridge();
    void disconnectBridge();
	bool isConnected();
	void timerCallback() override;
	void resetOrientation();

	float getRoll();
	float getPitch();
	float getYaw();
	float getRollOSC();
	float getPitchOSC();
	float getYawOSC();

	void setupRollOSC(String address, float min, float max);
	void setupPitchOSC(String address, float min, float max);
	void setupYawOSC(String address, float min, float max);
	void setupIp(String address, int port);

	int BaudR, PortN;
	bool m_rollMuted = false;
	bool m_pitchMuted = false;
	bool m_yawMuted = false;
       
private:
	StringPairArray portlist;

	int port_number, port_index, port_state;
	char readBuffer[128];
	StringArray m_quatsReceived;
	float qW, qX, qY, qZ;
	float qlW, qlX, qlY, qlZ;
	float qbW = 1, qbX = 0, qbY = 0, qbZ = 0;

	float m_roll, m_pitch, m_yaw;
	float m_rollOscMin, m_pitchOscMin, m_yawOscMin;
	float m_rollOscMax, m_pitchOscMax, m_yawOscMax;
	String m_rollOscAddress, m_pitchOscAddress, m_yawOscAddress;
	float m_rollOSC, m_pitchOSC, m_yawOSC;

	bool m_connected = false;

	String m_ipAddress = "127.0.0.1";
	int m_oscPortNumber = 9001;

	OSCSender sender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Bridge)
};        
