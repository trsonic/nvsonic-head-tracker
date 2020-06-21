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
	void updateEuler();

	float getRoll();
	float getPitch();
	float getYaw();
	float getRollOSC();
	float getPitchOSC();
	float getYawOSC();

	void setupQuatsOSC(bool isActive, String address, Array<int> order, Array<int> signs);
	void setupRollOSC(bool isActive, String address, float min, float max);
	void setupPitchOSC(bool isActive, String address, float min, float max);
	void setupYawOSC(bool isActive, String address, float min, float max);
	void setupRpyOSC(bool isActive, String address, String key);
	void setupIp(String address, int port);

	int BaudR, PortN;       
private:
	StringPairArray portlist;

	int port_number, port_index, port_state;
	char readBuffer[128];
	StringArray m_quatsReceived;
	float qW, qX, qY, qZ;
	float qlW, qlX, qlY, qlZ;
	float qbW = 1, qbX = 0, qbY = 0, qbZ = 0;

	float m_roll, m_pitch, m_yaw;
	bool m_quatsActive, m_rollActive, m_pitchActive, m_yawActive, m_rpyActive;
	String m_quatsOscAddress;
	Array<int> m_quatsOrder, m_quatsSigns;
	String m_rollOscAddress, m_pitchOscAddress, m_yawOscAddress;
	float m_rollOscMin, m_pitchOscMin, m_yawOscMin;
	float m_rollOscMax, m_pitchOscMax, m_yawOscMax;
	String m_rpyOscAddress, m_rpyOscKey;
	float m_rollOSC, m_pitchOSC, m_yawOSC;

	bool m_connected = false;
	String m_ipAddress;
	int m_oscPortNumber;
	OSCSender sender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Bridge)
};        
