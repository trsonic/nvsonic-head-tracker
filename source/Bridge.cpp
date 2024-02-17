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
    sender.connect (m_ipAddress, m_oscPortNumber);
}

Bridge::~Bridge()
{
    if (m_serialPortConnected)
    {
        disconnectSerial();
    }
    disconnectOscReceiver();
    sender.disconnect();
}

bool Bridge::connectOscReceiver()
{
    bool isConnected = connect (OSCinPortN); // osc input at port 8888
    addListener (this);
    return isConnected;
}

void Bridge::disconnectOscReceiver()
{
    disconnect();
}

void Bridge::oscMessageReceived (const juce::OSCMessage& message)
{
    if (message.getAddressPattern().toString() == OSCinAddress && message.size() == 4)
    {
        //qlW = message[0].getFloat32();
        //qlX = message[1].getFloat32();
        //qlY = message[2].getFloat32();
        //qlZ = message[3].getFloat32();
        qlW = message[0].getFloat32();
        qlX = message[1].getFloat32();
        qlY = message[3].getFloat32();
        qlZ = -message[2].getFloat32();
        pushQuaternionVector();
    }
}

void Bridge::oscBundleReceived (const juce::OSCBundle& bundle)
{
    juce::OSCBundle::Element elem = bundle[0];
    oscMessageReceived (elem.getMessage());
}

juce::StringArray Bridge::getPortInfo()
{
    port_number = comEnumerate();
    for (port_index = 0; port_index < port_number; port_index++)
        portlist.set (comGetInternalName (port_index), comGetPortName (port_index));
    return portlist.getAllValues();
}

bool Bridge::connectSerial()
{
    port_state = comOpen (PortN, BaudR);
    if (port_state == 1)
    {
        m_serialPortConnected = true;
        startTimer (10);
        return true;
    }
    else
    {
        return false;
    }
}

void Bridge::disconnectSerial()
{
    comClose (PortN);
    m_serialPortConnected = false;
    stopTimer();
}

bool Bridge::isSerialConnected()
{
    return m_serialPortConnected;
}

void Bridge::timerCallback()
{
    if (m_serialPortConnected)
    {
        char readBuffer[128];
        comRead (PortN, readBuffer, 128);

        if (strlen (readBuffer) != 0)
        {
            juce::StringArray m_quatsReceived = juce::StringArray::fromTokens (readBuffer, ",", "\"");
            if (m_quatsReceived.size() == 4)
            {
                qlW = m_quatsReceived[0].getFloatValue();
                qlX = m_quatsReceived[1].getFloatValue();
                qlY = m_quatsReceived[2].getFloatValue();
                qlZ = m_quatsReceived[3].getFloatValue();
                pushQuaternionVector();
            }
        }
    }
}

void Bridge::pushQuaternionVector()
{
    // normalization (just in case)
    double magnitude = sqrt (qlW * qlW + qlX * qlX + qlY * qlY + qlZ * qlZ);
    qlW /= magnitude;
    qlX /= magnitude;
    qlY /= magnitude;
    qlZ /= magnitude;

    qW = qbW * qlW + qbX * qlX + qbY * qlY + qbZ * qlZ;
    qX = qbW * qlX - qbX * qlW - qbY * qlZ + qbZ * qlY;
    qY = qbW * qlY + qbX * qlZ - qbY * qlW - qbZ * qlX;
    qZ = qbW * qlZ - qbX * qlY + qbY * qlX - qbZ * qlW;

    if (m_quatsActive)
    {
        const juce::Array<float> quats = { qW, qX, qY, qZ };
        if (m_quatsOrder.size() == 4 && m_quatsSigns.size() == 4)
        {
            sender.send (m_quatsOscAddress,
                m_quatsSigns[0] * quats[m_quatsOrder[0]],
                m_quatsSigns[1] * quats[m_quatsOrder[1]],
                m_quatsSigns[2] * quats[m_quatsOrder[2]],
                m_quatsSigns[3] * quats[m_quatsOrder[3]]);
        }
    }

    updateEuler();

    // Map and send rpy OSC
    m_rollOSC = (float) juce::jmap (m_roll, (float) -180, (float) 180, m_rollOscMin, m_rollOscMax);
    m_pitchOSC = (float) juce::jmap (m_pitch, (float) -180, (float) 180, m_pitchOscMin, m_pitchOscMax);
    m_yawOSC = (float) juce::jmap (m_yaw, (float) -180, (float) 180, m_yawOscMin, m_yawOscMax);
    if (m_rollActive)
        sender.send (m_rollOscAddress, m_rollOSC);
    if (m_pitchActive)
        sender.send (m_pitchOscAddress, m_pitchOSC);
    if (m_yawActive)
        sender.send (m_yawOscAddress, m_yawOSC);
    if (m_rpyActive)
    {
        if (m_rpyOscKey == "rpy")
            sender.send (m_rpyOscAddress, m_rollOSC, m_pitchOSC, m_yawOSC);
        else if (m_rpyOscKey == "ypr")
            sender.send (m_rpyOscAddress, m_yawOSC, m_pitchOSC, m_rollOSC);
        else if (m_rpyOscKey == "pry")
            sender.send (m_rpyOscAddress, m_pitchOSC, m_rollOSC, m_yawOSC);
        else if (m_rpyOscKey == "yrp")
            sender.send (m_rpyOscAddress, m_yawOSC, m_rollOSC, m_pitchOSC);
        else if (m_rpyOscKey == "ryp")
            sender.send (m_rpyOscAddress, m_rollOSC, m_yawOSC, m_pitchOSC);
        else if (m_rpyOscKey == "pyr")
            sender.send (m_rpyOscAddress, m_pitchOSC, m_yawOSC, m_rollOSC);
    }
}

void Bridge::resetOrientation()
{
    qbW = qlW;
    qbX = qlX;
    qbY = qlY;
    qbZ = qlZ;
}

void Bridge::updateEuler()
{
    const juce::Array<float> quats = { qW, qX, qY, qZ };

    qW = quats[0];
    qX = quats[2];
    qY = quats[1];
    qZ = quats[3];

    // thanks to Charles Verron (https://www.noisemakers.fr/) for providing the code snippet used below

    double test = qX * qZ + qY * qW;
    if (test > 0.499999)
    {
        // singularity at north pole
        m_yaw = 2 * atan2 (qX, qW);
        m_pitch = juce::MathConstants<double>::pi / 2;
        m_roll = 0;
        return;
    }
    if (test < -0.499999)
    {
        // singularity at south pole
        m_yaw = -2 * atan2 (qX, qW);
        m_pitch = -juce::MathConstants<double>::pi / 2;
        m_roll = 0;
        return;
    }
    double sqx = qX * qX;
    double sqy = qZ * qZ;
    double sqz = qY * qY;

    m_yaw = atan2 (2 * qZ * qW - 2 * qX * qY, 1 - 2 * sqy - 2 * sqz);
    m_pitch = asin (2 * test);
    m_roll = atan2 (2 * qX * qW - 2 * qZ * qY, 1 - 2 * sqx - 2 * sqz);

    m_yaw *= -1.0f;

    m_yaw = juce::radiansToDegrees (m_yaw);
    m_pitch = juce::radiansToDegrees (m_pitch);
    m_roll = juce::radiansToDegrees (m_roll);
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

void Bridge::setupQuatsOSC (bool isActive, juce::String address, juce::Array<int> order, juce::Array<int> signs)
{
    m_quatsActive = isActive;
    m_quatsOscAddress = address;
    m_quatsOrder = order;
    m_quatsSigns = signs;
}

void Bridge::setupRollOSC (bool isActive, juce::String address, float min, float max)
{
    m_rollActive = isActive;
    m_rollOscAddress = address;
    m_rollOscMin = min;
    m_rollOscMax = max;
}

void Bridge::setupPitchOSC (bool isActive, juce::String address, float min, float max)
{
    m_pitchActive = isActive;
    m_pitchOscAddress = address;
    m_pitchOscMin = min;
    m_pitchOscMax = max;
}

void Bridge::setupYawOSC (bool isActive, juce::String address, float min, float max)
{
    m_yawActive = isActive;
    m_yawOscAddress = address;
    m_yawOscMin = min;
    m_yawOscMax = max;
}

void Bridge::setupRpyOSC (bool isActive, juce::String address, juce::String key)
{
    m_rpyActive = isActive;
    m_rpyOscAddress = address;
    m_rpyOscKey = key;
}

void Bridge::setupIp (juce::String address, int port)
{
    if (m_ipAddress != address || m_oscPortNumber != port)
    {
        m_ipAddress = address;
        m_oscPortNumber = port;
        sender.disconnect();
        sender.connect (m_ipAddress, m_oscPortNumber);
    }
}
