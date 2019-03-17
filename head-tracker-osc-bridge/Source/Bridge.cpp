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
    sender.connect ("127.0.0.1", 9001);
}

Bridge::~Bridge()
{
	if (connected == true)
	{
		Disconnect();
	}   
    stopTimer();
}

StringArray Bridge::GetPortInfo()
{
    port_number = comEnumerate();
    for(port_index=0; port_index < port_number; port_index++)
        portlist.set(comGetInternalName(port_index),comGetPortName(port_index));
    return portlist.getAllValues();
}

bool Bridge::Connect()
{
    port_state=comOpen(PortN,BaudR);
	if(port_state == 1)
		{
            connected = true;
         	return true;
	}
	else
    {
            return false;
    }		
}


void Bridge::Disconnect()
{
    comClose(PortN);
    connected = false;
}

void Bridge::timerCallback()
{
	if(connected) {
        
        memset(&readBuffer[0], 0, sizeof(readBuffer));
        comRead(PortN, readBuffer, 128);
        
        if(strlen(readBuffer) != 0)
        {
            // line processing
            quaternions = StringArray::fromTokens(readBuffer, ",", "\"");
			// DBG(String(quaternions.size()));
            if(quaternions.size() == 4) // size > 4 is probably the result of overflowed buffer, this should not happen in normal operation
            {
                qlW = quaternions[0].getFloatValue();
                qlX = quaternions[1].getFloatValue();
                qlY = quaternions[2].getFloatValue();
                qlZ = quaternions[3].getFloatValue();
                //std::cout << "W: " << String(qlW) << " X: " << String(qlX) << " Y: " << String(qlW) << " Z: " << String(qlZ) << "\n";
                quaternions.clearQuick();

                qW = qbW * qlW + qbX * qlX + qbY * qlY + qbZ * qlZ;
                qX = qbW * qlX - qbX * qlW - qbY * qlZ + qbZ * qlY;
                qY = qbW * qlY + qbX * qlZ - qbY * qlW - qbZ * qlX;
                qZ = qbW * qlZ - qbX * qlY + qbY * qlX - qbZ * qlW;

                // roll (y-axis rotation)
                double sinr_cosp = +2.0 * (qW * qX + qY * qZ);
                double cosr_cosp = +1.0 - 2.0 * (qX * qX + qY * qY);
                PitchOUT = atan2(sinr_cosp, cosr_cosp)  * (180 / double_Pi);

                // pitch (x-axis rotation)
                double sinp = +2.0 * (qW * qY - qZ * qX);
                if (fabs(sinp) >= 1)
                    RollOUT = copysign(double_Pi / 2, sinp)  * (180 / double_Pi); // use 90 degrees if out of range
                else
                    RollOUT = asin(sinp)  * (180 / double_Pi);

                // yaw (z-axis rotation)
                double siny_cosp = +2.0 * (qW * qZ + qX * qY);
                double cosy_cosp = +1.0 - 2.0 * (qY * qY + qZ * qZ);
                YawOUT = atan2(siny_cosp, cosy_cosp)  * (180 / double_Pi);

                // Sign change
                RollOUT = RollOUT * -1;
                PitchOUT = PitchOUT * -1;
                //YawOUT = YawOUT * -1;

                RollOutput = String(RollOUT * -1,1);
                PitchOutput = String(PitchOUT * -1,1);
                YawOutput = String(YawOUT * -1,1);

                // Map and send OSC
                RollOSC     = (float) jmap(RollOUT, (float) -180, (float) 180, (float) 0, (float) 1);
                PitchOSC    = (float) jmap(PitchOUT, (float) -180, (float) 180, (float) 0, (float) 1);
                YawOSC      = (float) jmap(YawOUT, (float) -180, (float) 180, (float) 0, (float) 1);
                if (AXmuted == false) sender.send ("/roll/", (float) RollOSC);
                if (AYmuted == false) sender.send ("/pitch/", (float) PitchOSC);
                if (AZmuted == false) sender.send ("/yaw/", (float) YawOSC);
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
