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

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
	// buttons
	brefresh.setButtonText("Refresh");
	brefresh.setColour(TextButton::buttonColourId, clblue);
	brefresh.addListener(this);
	addAndMakeVisible(brefresh);

	bconnect.setButtonText("Connect");
	bconnect.setColour(TextButton::buttonColourId, clblue);
	bconnect.addListener(this);
	addAndMakeVisible(bconnect);

	breset.setButtonText("Reset");
	breset.setColour(TextButton::buttonColourId, clblue);
	addAndMakeVisible(breset);
	breset.addListener(this);

	bmAX.setButtonText("M");
	bmAX.setColour(TextButton::buttonColourId, clblue);
	addAndMakeVisible(bmAX);
	bmAX.addListener(this);

	bmAY.setButtonText("M");
	bmAY.setColour(TextButton::buttonColourId, clblue);
	addAndMakeVisible(bmAY);
	bmAY.addListener(this);

	bmAZ.setButtonText("M");
	bmAZ.setColour(TextButton::buttonColourId, clblue);
	addAndMakeVisible(bmAZ);
	bmAZ.addListener(this);

	// combobox
	cb_portlist.setEditableText(false);
	cb_portlist.setJustificationType(Justification::centred);
	cb_portlist.setTextWhenNothingSelected(String("select device"));
	cb_portlist.addListener(this);
	addAndMakeVisible(cb_portlist);

	// refresh port list
	CBox_portlist = bridge.GetPortInfo();
	cb_portlist.clear();
	cb_portlist.addItemList(CBox_portlist, 1);

	//labels
	addAndMakeVisible(AxisX);
	AxisX.setFont(labelfont.withPointHeight(13));
	AxisX.setJustificationType(Justification::centredLeft);
	AxisX.setColour(Label::textColourId, clrblue);

	addAndMakeVisible(AxisY);
	AxisY.setFont(labelfont.withPointHeight(13));
	AxisY.setJustificationType(Justification::centredLeft);
	AxisY.setColour(Label::textColourId, clrblue);

	addAndMakeVisible(AxisZ);
	AxisZ.setFont(labelfont.withPointHeight(13));
	AxisZ.setJustificationType(Justification::centredLeft);
	AxisZ.setColour(Label::textColourId, clrblue);

	AxisY.setText("Roll (Y):", dontSendNotification);
	AxisX.setText("Pitch (X):", dontSendNotification);
	AxisZ.setText("Yaw (Z):", dontSendNotification);


	addAndMakeVisible(AxisXval);
	AxisXval.setFont(labelfont.withPointHeight(13));
	AxisXval.setJustificationType(Justification::centredRight);
	AxisXval.setColour(Label::textColourId, clrblue);

	addAndMakeVisible(AxisYval);
	AxisYval.setFont(labelfont.withPointHeight(13));
	AxisYval.setJustificationType(Justification::centredRight);
	AxisYval.setColour(Label::textColourId, clrblue);

	addAndMakeVisible(AxisZval);
	AxisZval.setFont(labelfont.withPointHeight(13));
	AxisZval.setJustificationType(Justification::centredRight);
	AxisZval.setColour(Label::textColourId, clrblue);

	// look and feeeel
	brefresh.setLookAndFeel(&SMLF);
	bconnect.setLookAndFeel(&SMLF);
	breset.setLookAndFeel(&SMLF);
	bmAX.setLookAndFeel(&SMLF);
	bmAY.setLookAndFeel(&SMLF);
	bmAZ.setLookAndFeel(&SMLF);
	cb_portlist.setLookAndFeel(&SMLF);

	startTimerHz(20);
	setSize(300, 550);
}

MainComponent::~MainComponent()
{
	stopTimer();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
	auto titlespace = getLocalBounds().removeFromTop(35);
    #ifdef _WIN32
        auto titletextspaceA = getLocalBounds().removeFromTop(37).removeFromLeft(100).removeFromRight(90);
    #else
        auto titletextspaceA = getLocalBounds().removeFromTop(45).removeFromLeft(100).removeFromRight(90);
    #endif
    auto titletextspaceB = getLocalBounds().removeFromTop(40).removeFromRight(200).removeFromLeft(190);

	auto seriallabelspace = getLocalBounds().removeFromBottom(505).removeFromTop(60)
		.removeFromLeft(290).removeFromRight(280); // serial port label bounds
	auto statuslabelspace = getLocalBounds().removeFromBottom(350).removeFromTop(60)
		.removeFromLeft(290).removeFromRight(280); // status label bounds
	auto osclabelspace = getLocalBounds().removeFromBottom(198).removeFromTop(60)
		.removeFromLeft(290).removeFromRight(280); // osc label bounds

	// background
	g.fillAll(cdark);

	// title
	g.setColour(cdark);
	g.fillRect(titlespace);
    
	g.setColour(clblue);
	g.setFont(titlefontA.withPointHeight(23));
	g.drawFittedText("nvsonic", titletextspaceA, Justification::centredLeft, 1);

	g.setColour(clrblue);
	g.setFont(titlefontB.withPointHeight(16));
	g.drawFittedText("Head Tracker OSC Bridge", titletextspaceB, Justification::centredRight, 1);

	// labels
	g.setColour(clrblue);
	g.fillRoundedRectangle(seriallabelspace.toFloat(), 3.0f);
	g.fillRoundedRectangle(statuslabelspace.toFloat(), 3.0f);
	g.fillRoundedRectangle(osclabelspace.toFloat(), 3.0f);

	g.setColour(cdark);
	g.setFont(titlefontB.withPointHeight(15));
	g.drawText("Serial Port Configuration", seriallabelspace.removeFromRight(270), Justification::left, 1);
	g.drawText("IMU Orientation", statuslabelspace.removeFromRight(270), Justification::left, 1);
	g.drawText("OSC Configuration", osclabelspace.removeFromRight(270), Justification::left, 1);

	// other texts
	g.setFont(titlefontB.withPointHeight(14));
	g.setColour(clrblue);
	g.drawText("Port List:", getLocalBounds().removeFromBottom(388).removeFromTop(30)
		.removeFromRight(290).removeFromLeft(135), Justification::centred, 1);

	g.setFont(titlefontB.withPointHeight(15));
	g.drawText("IP Adress: 127.0.0.1", getLocalBounds().removeFromBottom(moutpos - 0).removeFromTop(30)
		.removeFromRight(290 - marginleft).removeFromLeft(280 - marginleft), Justification::left, 1);

	g.drawText("UDP Port #: 9001", getLocalBounds().removeFromBottom(moutpos - 25).removeFromTop(30)
		.removeFromRight(290 - marginleft).removeFromLeft(280 - marginleft), Justification::left, 1);

	// version number & authors
	g.setFont(titlefontB.withPointHeight(12));
	g.setColour(clblue);
	g.drawText("version 2.0", getLocalBounds().removeFromBottom(35).removeFromTop(30)
		.removeFromRight(290 - marginleft).removeFromLeft(280 - marginleft), Justification::left, 1);
	g.drawText("\u00A9 2019 Tomasz Rudzki, Jacek Majer", getLocalBounds().removeFromBottom(35).removeFromTop(30)
		.removeFromRight(290 - marginleft).removeFromLeft(280 - marginleft), Justification::right, 1);

	// icons
	g.drawImageAt(iserial, 210, 62);
	g.drawImageAt(iaxis, 232, 211);
	g.drawImageAt(iosc, 220, 357);

}

void MainComponent::resized()
{
	const int buttonHeight = 40;

	// buttons
	brefresh.setBounds(getLocalBounds().removeFromBottom(435).removeFromTop(buttonHeight)
		.removeFromRight(290).removeFromLeft(135));

	bconnect.setBounds(getLocalBounds().removeFromBottom(435).removeFromTop(buttonHeight)
		.removeFromLeft(290).removeFromRight(135));

	breset.setBounds(getLocalBounds().removeFromBottom(281).removeFromTop(buttonHeight - 17)
		.removeFromLeft(290).removeFromRight(135));

	bmAX.setBounds(getLocalBounds().removeFromBottom(statuspos - 7).removeFromTop(16)
		.removeFromRight(290).removeFromLeft(22));

	bmAY.setBounds(getLocalBounds().removeFromBottom(statuspos - 25 - 7).removeFromTop(16)
		.removeFromRight(290).removeFromLeft(22));

	bmAZ.setBounds(getLocalBounds().removeFromBottom(statuspos - 50 - 7).removeFromTop(16)
		.removeFromRight(290).removeFromLeft(22));

	//comboBox
	cb_portlist.setBounds(getLocalBounds().removeFromBottom(388).removeFromTop(30)
		.removeFromLeft(290).removeFromRight(135));

	// labels
	AxisY.setBounds(getLocalBounds().removeFromBottom(statuspos).removeFromTop(30)
		.withTrimmedLeft(35).withTrimmedRight(160));

	AxisX.setBounds(getLocalBounds().removeFromBottom(statuspos - 25).removeFromTop(30)
		.withTrimmedLeft(35).withTrimmedRight(160));

	AxisZ.setBounds(getLocalBounds().removeFromBottom(statuspos - 50).removeFromTop(30)
		.withTrimmedLeft(35).withTrimmedRight(160));

	AxisYval.setBounds(getLocalBounds().removeFromBottom(statuspos).removeFromTop(30)
		.withTrimmedLeft(100).withTrimmedRight(150));

	AxisXval.setBounds(getLocalBounds().removeFromBottom(statuspos - 25).removeFromTop(30)
		.withTrimmedLeft(100).withTrimmedRight(150));

	AxisZval.setBounds(getLocalBounds().removeFromBottom(statuspos - 50).removeFromTop(30)
		.withTrimmedLeft(100).withTrimmedRight(150));
}

void MainComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	bridge.PortN = cb_portlist.getSelectedItemIndex();

}

void MainComponent::buttonClicked(Button* buttonThatWasClicked)
{

	if (buttonThatWasClicked == &brefresh)
	{
		CBox_portlist = bridge.GetPortInfo();
		cb_portlist.clear();
		cb_portlist.addItemList(CBox_portlist, 1);
	}
	else if (buttonThatWasClicked == &bconnect)
	{
		if (bconnect.getButtonText() == "Connect")
		{
			if (bridge.Connect())
			{
				bconnect.setColour(TextButton::buttonColourId, cgrnsh);
				bconnect.setButtonText("Disconnect");
			}
		}
		else
		{
			bridge.Disconnect();
			bconnect.setColour(TextButton::buttonColourId, clblue);
			bconnect.setButtonText("Connect");
		}
	}
	else if (buttonThatWasClicked == &breset && bridge.connected == true)
	{
		bridge.resetOrientation();
	}
	else if (buttonThatWasClicked == &bmAX)
	{
		// mute code
		if (bridge.AXmuted == false)
		{
			bridge.AXmuted = true;
			bmAX.setColour(TextButton::buttonColourId, cred);
		}
		else {
			bridge.AXmuted = false;
			bmAX.setColour(TextButton::buttonColourId, clblue);
		}
	}
	else if (buttonThatWasClicked == &bmAY)
	{
		// mute code
		if (bridge.AYmuted == false)
		{
			bridge.AYmuted = true;
			bmAY.setColour(TextButton::buttonColourId, cred);
		}
		else {
			bridge.AYmuted = false;
			bmAY.setColour(TextButton::buttonColourId, clblue);
		}
	}
	else if (buttonThatWasClicked == &bmAZ)
	{
		// mute code
		if (bridge.AZmuted == false)
		{
			bridge.AZmuted = true;
			bmAZ.setColour(TextButton::buttonColourId, cred);
		}
		else {
			bridge.AZmuted = false;
			bmAZ.setColour(TextButton::buttonColourId, clblue);
		}
	}
}

void MainComponent::timerCallback()
{
	if (bridge.connected == true)
	{
		AxisYval.setText(bridge.RollOutput + "°", dontSendNotification);
		AxisXval.setText(bridge.PitchOutput + "°", dontSendNotification);
		AxisZval.setText(bridge.YawOutput + "°", dontSendNotification);
	}
	else {
		AxisYval.setText("", dontSendNotification);
		AxisXval.setText("", dontSendNotification);
		AxisZval.setText("", dontSendNotification);
	}
}
