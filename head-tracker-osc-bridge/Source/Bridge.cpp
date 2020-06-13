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

#include "Bridge.h"

Bridge::Bridge()
{
    BaudR = 115200;
    startTimer(10);
}

Bridge::~Bridge()
{
	if (m_connected == true)
	{
		disconnectBridge();
	}   
    stopTimer();
}

StringArray Bridge::getPortInfo()
{
    port_number = comEnumerate();
    for(port_index=0; port_index < port_number; port_index++)
        portlist.set(comGetInternalName(port_index),comGetPortName(port_index));
    return portlist.getAllValues();
}

bool Bridge::connectBridge()
{
    port_state = comOpen(PortN, BaudR);
    if (port_state == 1)
    {
        sender.connect(m_ipAddress, m_oscPortNumber);
        m_connected = true;
        return true;
    }
    else
    {
        return false;
    }
}

void Bridge::disconnectBridge()
{
    comClose(PortN);
    sender.disconnect();
    m_connected = false;
}

bool Bridge::isConnected()
{
    return m_connected;
}

void Bridge::timerCallback()
{
	if(m_connected)
    {    
        memset(&readBuffer[0], 0, sizeof(readBuffer));
        comRead(PortN, readBuffer, 128);
        
        if(strlen(readBuffer) != 0)
        {
            m_quatsReceived = StringArray::fromTokens(readBuffer, ",", "\"");
            if(m_quatsReceived.size() == 4)
            {
                qlW = m_quatsReceived[0].getFloatValue();
                qlX = m_quatsReceived[1].getFloatValue();
                qlY = m_quatsReceived[2].getFloatValue();
                qlZ = m_quatsReceived[3].getFloatValue();
                m_quatsReceived.clearQuick();

                qW = qbW * qlW + qbX * qlX + qbY * qlY + qbZ * qlZ;
                qX = qbW * qlX - qbX * qlW - qbY * qlZ + qbZ * qlY;
                qY = qbW * qlY + qbX * qlZ - qbY * qlW - qbZ * qlX;
                qZ = qbW * qlZ - qbX * qlY + qbY * qlX - qbZ * qlW;

                // roll (y-axis rotation)
                double sinp = +2.0 * (qW * qY - qZ * qX);
                if (fabs(sinp) >= 1)
                    m_roll = (float) copysign(double_Pi / 2, sinp) * (180 / double_Pi); // use 90 degrees if out of range
                else
                    m_roll = (float) asin(sinp) * (180 / double_Pi);

                // pitch (x-axis rotation)
                double sinr_cosp = +2.0 * (qW * qX + qY * qZ);
                double cosr_cosp = +1.0 - 2.0 * (qX * qX + qY * qY);
                m_pitch = (float) atan2(sinr_cosp, cosr_cosp)  * (180 / double_Pi);

                // yaw (z-axis rotation)
                double siny_cosp = +2.0 * (qW * qZ + qX * qY);
                double cosy_cosp = +1.0 - 2.0 * (qY * qY + qZ * qZ);
                m_yaw = (float) atan2(siny_cosp, cosy_cosp)  * (180 / double_Pi) * -1;

                // console output
                // DBG("Roll: " + String(m_roll) + ", Pitch: " + String(m_pitch) + ", Yaw: " + String(m_yaw));

                // Map and send OSC
                m_rollOSC     = (float) jmap(m_roll, (float) -180, (float) 180, m_rollOscMin, m_rollOscMax);
                m_pitchOSC    = (float) jmap(m_pitch, (float) -180, (float) 180, m_pitchOscMin, m_pitchOscMax);
                m_yawOSC      = (float) jmap(m_yaw, (float) -180, (float) 180, m_yawOscMin, m_yawOscMax);
                if (m_rollActive) sender.send(m_rollOscAddress, m_rollOSC);
                if (m_pitchActive) sender.send(m_pitchOscAddress, m_pitchOSC);
                if (m_yawActive) sender.send(m_yawOscAddress, m_yawOSC);

                if (m_rpyActive)
                {
                    float msg1, msg2, msg3;
                    if (m_rpyOscKey == "rpy")
                    {
                        msg1 = m_rollOSC; msg2 = m_pitchOSC; msg3 = m_yawOSC;
                    }
                    else if (m_rpyOscKey == "ypr")
                    {
                        msg1 = m_yawOSC; msg2 = m_pitchOSC; msg3 = m_rollOSC;
                    }
                    else if (m_rpyOscKey == "pry")
                    {
                        msg1 = m_pitchOSC; msg2 = m_rollOSC; msg3 = m_yawOSC;
                    }
                    else if (m_rpyOscKey == "yrp")
                    {
                        msg1 = m_yawOSC; msg2 = m_rollOSC; msg3 = m_pitchOSC;
                    }
                    else if (m_rpyOscKey == "ryp")
                    {
                        msg1 = m_rollOSC; msg2 = m_yawOSC; msg3 = m_pitchOSC;
                    }
                    else if (m_rpyOscKey == "pyr")
                    {
                        msg1 = m_pitchOSC; msg2 = m_yawOSC; msg3 = m_rollOSC;
                    }
                    sender.send(m_rpyOscAddress, msg1, msg2, msg3);
                }
				
				// send rpy
				// sender2.send("/rendering/htrpy", RollOUT, PitchOUT, YawOUT);

				// send quaternions
				//sender.send("/rendering/quaternions/", qW, qX, qY, qZ);
            }
        }
	}
}

void Bridge::resetOrientation()
{
	qbW = qlW;
	qbX = qlX;
	qbY = qlY;
	qbZ = qlZ;
}

float Bridge::getRoll()
{
    return m_roll;
}

float Bridge::getPitch()
{
    return m_pitch;
}

float Bridge::getYaw()
{
    return m_yaw;
}

float Bridge::getRollOSC()
{
    return m_rollOSC;
}

float Bridge::getPitchOSC()
{
    return m_pitchOSC;
}

float Bridge::getYawOSC()
{
    return m_yawOSC;
}

void Bridge::setupRollOSC(bool isActive, String address, float min, float max)
{
    m_rollActive = isActive;
    m_rollOscAddress = address;
    m_rollOscMin = min;
    m_rollOscMax = max;
}

void Bridge::setupPitchOSC(bool isActive, String address, float min, float max)
{
    m_pitchActive = isActive;
    m_pitchOscAddress = address;
    m_pitchOscMin = min;
    m_pitchOscMax = max;
}

void Bridge::setupYawOSC(bool isActive, String address, float min, float max)
{
    m_yawActive = isActive;
    m_yawOscAddress = address;
    m_yawOscMin = min;
    m_yawOscMax = max;
}

void Bridge::setupRpyOSC(bool isActive, String address, String key)
{
    m_rpyActive = isActive;
    m_rpyOscAddress = address;
    m_rpyOscKey = key;
}

void Bridge::setupIp(String address, int port)
{
    if (m_ipAddress != address || m_oscPortNumber != port)
    {
        m_ipAddress = address;
        m_oscPortNumber = port;
        sender.disconnect();
        sender.connect(m_ipAddress, m_oscPortNumber);
    }
}