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

MainComponent::MainComponent()
{
	m_refreshButton.setButtonText("Refresh");
	m_refreshButton.setColour(TextButton::buttonColourId, clblue);
	m_refreshButton.setLookAndFeel(&SMLF);
	m_refreshButton.addListener(this);
	addAndMakeVisible(m_refreshButton);

	m_connectButton.setButtonText("Connect");
	m_connectButton.setColour(TextButton::buttonColourId, clblue);
	m_connectButton.setColour(TextButton::buttonOnColourId, cgrnsh);
	m_connectButton.setLookAndFeel(&SMLF);
	m_connectButton.addListener(this);
	addAndMakeVisible(m_connectButton);

	m_resetButton.setButtonText("Reset");
	m_resetButton.setColour(TextButton::buttonColourId, clblue);
	m_resetButton.setLookAndFeel(&SMLF);
	m_resetButton.addListener(this);
	addAndMakeVisible(m_resetButton);

	m_rollOscMute.setButtonText("R");
	m_rollOscMute.setColour(TextButton::buttonColourId, clblue);
	m_rollOscMute.setLookAndFeel(&SMLF);
	m_rollOscMute.addListener(this);
	addAndMakeVisible(m_rollOscMute);

	m_pitchOscMute.setButtonText("P");
	m_pitchOscMute.setColour(TextButton::buttonColourId, clblue);
	m_pitchOscMute.setLookAndFeel(&SMLF);
	m_pitchOscMute.addListener(this);
	addAndMakeVisible(m_pitchOscMute);

	m_yawOscMute.setButtonText("Y");
	m_yawOscMute.setColour(TextButton::buttonColourId, clblue);
	m_yawOscMute.setLookAndFeel(&SMLF);
	m_yawOscMute.addListener(this);
	addAndMakeVisible(m_yawOscMute);

	m_rpyOscMute.setButtonText("3");
	m_rpyOscMute.setColour(TextButton::buttonColourId, clblue);
	m_rpyOscMute.setLookAndFeel(&SMLF);
	m_rpyOscMute.addListener(this);
	addAndMakeVisible(m_rpyOscMute);

	m_portlistCB.setEditableText(false);
	m_portlistCB.setJustificationType(Justification::centred);
	m_portlistCB.setTextWhenNothingSelected(String("select device"));
	m_portlistCB.setLookAndFeel(&SMLF);
	m_portlistCB.addListener(this);
	addAndMakeVisible(m_portlistCB);
	refreshPortList();

	m_yprOrderCB.setEditableText(false);
	m_yprOrderCB.setJustificationType(Justification::centred);
	m_yprOrderCB.setTextWhenNothingSelected(String("yaw-pitch-roll"));
	m_yprOrderCB.setLookAndFeel(&SMLF);
	m_yprOrderCB.addListener(this);
	addAndMakeVisible(m_yprOrderCB);

	m_oscPresetCB.setEditableText(false);
	m_oscPresetCB.setJustificationType(Justification::centred);
	m_oscPresetCB.setTextWhenNothingSelected(String("select preset"));
	m_oscPresetCB.setLookAndFeel(&SMLF);
	m_oscPresetCB.addListener(this);
	addAndMakeVisible(m_oscPresetCB);

	// labels
	Array<Label*> rpyValLabels;
	rpyValLabels.add(&m_rollLabel);
	rpyValLabels.add(&m_pitchLabel);
	rpyValLabels.add(&m_yawLabel);
	rpyValLabels.add(&m_rollOscVal);
	rpyValLabels.add(&m_pitchOscVal);
	rpyValLabels.add(&m_yawOscVal);

	for (int i = 0; i < rpyValLabels.size(); ++i)
	{
		rpyValLabels[i]->setFont(labelfont.withPointHeight(13));
		rpyValLabels[i]->setJustificationType(Justification::centredRight);
		rpyValLabels[i]->setColour(Label::textColourId, clrblue);
		addAndMakeVisible(rpyValLabels[i]);
	}

	Array<Label*> oscLabels;
	oscLabels.add(&m_rollOscAddress);
	oscLabels.add(&m_pitchOscAddress);
	oscLabels.add(&m_yawOscAddress);
	oscLabels.add(&m_rpyOscAddress);
	oscLabels.add(&m_rollOscMin);
	oscLabels.add(&m_pitchOscMin);
	oscLabels.add(&m_yawOscMin);
	oscLabels.add(&m_rollOscMax);
	oscLabels.add(&m_pitchOscMax);
	oscLabels.add(&m_yawOscMax);
	oscLabels.add(&m_ipAddress);
	oscLabels.add(&m_portNumber);

	for (int i = 0; i < oscLabels.size(); ++i)
	{
		oscLabels[i]->setEditable(false, true, false);
		oscLabels[i]->onTextChange = [this] { updateBridgeSettings(); };
		oscLabels[i]->setColour(Label::outlineColourId, clrblue);
		oscLabels[i]->setColour(Label::textColourId, clrblue);
		oscLabels[i]->setFont(labelfont.withPointHeight(13));
		oscLabels[i]->setJustificationType(Justification::centred);
		addAndMakeVisible(oscLabels[i]);
	}

	loadSettings();
	startTimerHz(20);
	setSize(300, 600);
}

MainComponent::~MainComponent()
{
	stopTimer();
}

void MainComponent::paint (Graphics& g)
{
	// background
	g.fillAll(cdark);
    
	// logo & title
	g.setColour(clblue);
	g.setFont(titlefontA.withPointHeight(25));
	g.drawText("nvsonic", 10, 0, 90, 38, Justification::bottomLeft);

	g.setColour(clrblue);
	g.setFont(titlefontB.withPointHeight(17));
	g.drawText("Head Tracker OSC Bridge", 100, 0, 190, 33, Justification::bottomRight);

	// labels
	Rectangle<float> serialLabelArea(10, 40, 280, 50);
	Rectangle<float> imuLabelArea(10, 180, 280, 50);
	Rectangle<float> oscLabelArea(10, 310, 280, 50);

	g.setColour(clrblue);
	g.fillRoundedRectangle(serialLabelArea, 3.0f);
	g.fillRoundedRectangle(imuLabelArea, 3.0f);
	g.fillRoundedRectangle(oscLabelArea, 3.0f);

	g.setColour(cdark);
	g.setFont(titlefontB.withPointHeight(15));
	g.drawText("Serial Port Configuration", serialLabelArea.removeFromRight(270), Justification::left);
	g.drawText("IMU Orientation", imuLabelArea.removeFromRight(270), Justification::left);
	g.drawText("OSC Configuration", oscLabelArea.removeFromRight(270), Justification::left);

	g.drawImageAt(iserial, 210, 52);
	//g.drawImageAt(iaxis, 225, 185);
	//g.drawImageAt(iosc, 220, 310);


	// other texts
	g.setFont(titlefontB.withPointHeight(14));
	g.setColour(clrblue);
	g.drawText("Port List:", 10, 140, 135, 30, Justification::centred);
	g.setFont(titlefontB.withPointHeight(13));
	g.drawText("Roll (Y):", 20, 240, 125, 20, Justification::centredLeft);
	g.drawText("Pitch (X):", 20, 260, 125, 20, Justification::centredLeft);
	g.drawText("Yaw (Z):", 20, 280, 125, 20, Justification::centredLeft);

	// version number & authors
	g.setFont(titlefontB.withPointHeight(12));
	g.setColour(clblue);
	g.drawText("version 3.0", 10, getHeight() - 30, 280, 20, Justification::bottomLeft);
	g.drawText("2020 Tomasz Rudzki, Jacek Majer", 10, getHeight() - 30, 280, 20, Justification::bottomRight);
}

void MainComponent::resized()
{
	m_refreshButton.setBounds(10, 100, 135, 30);
	m_connectButton.setBounds(155, 100, 135, 30);
	m_portlistCB.setBounds(155, 140, 135, 30);
	m_resetButton.setBounds(155, 240, 135, 60);

	m_rollLabel.setBounds(70, 240, 65, 20);
	m_pitchLabel.setBounds(70, 260, 65, 20);
	m_yawLabel.setBounds(70, 280, 65, 20);

	m_rollOscMute.setBounds(10, 370, 25, 25);
	m_pitchOscMute.setBounds(10, 400, 25, 25);
	m_yawOscMute.setBounds(10, 430, 25, 25);
	m_rpyOscMute.setBounds(10, 460, 25, 25);

	m_rollOscAddress.setBounds(40, 370, 105, 25);
	m_pitchOscAddress.setBounds(40, 400, 105, 25);
	m_yawOscAddress.setBounds(40, 430, 105, 25);
	m_rpyOscAddress.setBounds(40, 460, 105, 25);
	m_rollOscMin.setBounds(155, 370, 40, 25);
	m_pitchOscMin.setBounds(155, 400, 40, 25);
	m_yawOscMin.setBounds(155, 430, 40, 25);
	m_rollOscMax.setBounds(200, 370, 40, 25);
	m_pitchOscMax.setBounds(200, 400, 40, 25);
	m_yawOscMax.setBounds(200, 430, 40, 25);
	m_rollOscVal.setBounds(245, 370, 45, 25);
	m_pitchOscVal.setBounds(245, 400, 45, 25);
	m_yawOscVal.setBounds(245, 430, 45, 25);
	m_ipAddress.setBounds(10, 490, 135, 25);
	m_portNumber.setBounds(155, 490, 135, 25);

	m_yprOrderCB.setBounds(155, 460, 135, 25);
	m_oscPresetCB.setBounds(10, 520, 280, 25);
}

void MainComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	bridge.PortN = m_portlistCB.getSelectedItemIndex();
}

void MainComponent::buttonClicked(Button* buttonThatWasClicked)
{

	if (buttonThatWasClicked == &m_refreshButton)
	{
		refreshPortList();
	}
	else if (buttonThatWasClicked == &m_connectButton)
	{
		if (m_connectButton.getToggleState())
		{
			bridge.disconnectBridge();
			m_connectButton.setToggleState(false, dontSendNotification);
			m_connectButton.setButtonText("Connect");
			m_refreshButton.setEnabled(true);
			m_portlistCB.setEnabled(true);
			m_resetButton.setEnabled(false);
		}
		else
		{
			if (bridge.connectBridge())
			{
				m_connectButton.setToggleState(true, dontSendNotification);
				m_connectButton.setButtonText("Disconnect");
				m_refreshButton.setEnabled(false);
				m_portlistCB.setEnabled(false);
				m_resetButton.setEnabled(true);
			}
		}
	}
	else if (buttonThatWasClicked == &m_resetButton && bridge.isConnected())
	{
		bridge.resetOrientation();
	}
	else if (buttonThatWasClicked == &m_rollOscMute)
	{
		if (bridge.m_rollMuted == false)
		{
			bridge.m_rollMuted = true;
			m_rollOscMute.setColour(TextButton::buttonColourId, cred);
		}
		else
		{
			bridge.m_rollMuted = false;
			m_rollOscMute.setColour(TextButton::buttonColourId, clblue);
		}
	}
	else if (buttonThatWasClicked == &m_pitchOscMute)
	{
		if (bridge.m_pitchMuted == false)
		{
			bridge.m_pitchMuted = true;
			m_pitchOscMute.setColour(TextButton::buttonColourId, cred);
		}
		else
		{
			bridge.m_pitchMuted = false;
			m_pitchOscMute.setColour(TextButton::buttonColourId, clblue);
		}
	}
	else if (buttonThatWasClicked == &m_yawOscMute)
	{
		if (bridge.m_yawMuted == false)
		{
			bridge.m_yawMuted = true;
			m_yawOscMute.setColour(TextButton::buttonColourId, cred);
		}
		else
		{
			bridge.m_yawMuted = false;
			m_yawOscMute.setColour(TextButton::buttonColourId, clblue);
		}
	}
}

void MainComponent::timerCallback()
{
	if (bridge.isConnected())
	{
		m_rollLabel.setText(String(bridge.getRoll(),1) + "°", dontSendNotification);
		m_pitchLabel.setText(String(bridge.getPitch(),1) + "°", dontSendNotification);
		m_yawLabel.setText(String(bridge.getYaw(),1) + "°", dontSendNotification);
		m_rollOscVal.setText(String(bridge.getRollOSC(), 2), dontSendNotification);
		m_pitchOscVal.setText(String(bridge.getPitchOSC(), 2), dontSendNotification);
		m_yawOscVal.setText(String(bridge.getYawOSC(), 2), dontSendNotification);
	}
	else
	{
		m_rollLabel.setText("", dontSendNotification);
		m_pitchLabel.setText("", dontSendNotification);
		m_yawLabel.setText("", dontSendNotification);
		m_rollOscVal.setText("", dontSendNotification);
		m_pitchOscVal.setText("", dontSendNotification);
		m_yawOscVal.setText("", dontSendNotification);
	}
}

void MainComponent::refreshPortList()
{
	StringArray CBox_portlist = bridge.getPortInfo();
	m_portlistCB.clear();
	m_portlistCB.addItemList(CBox_portlist, 1);
}

void MainComponent::updateBridgeSettings()
{
	bridge.setupRollOSC(m_rollOscAddress.getText(), m_rollOscMin.getText().getFloatValue(), m_rollOscMax.getText().getFloatValue());
	bridge.setupPitchOSC(m_pitchOscAddress.getText(), m_pitchOscMin.getText().getFloatValue(), m_pitchOscMax.getText().getFloatValue());
	bridge.setupYawOSC(m_yawOscAddress.getText(), m_yawOscMin.getText().getFloatValue(), m_yawOscMax.getText().getFloatValue());
	bridge.setupIp(m_ipAddress.getText(), m_portNumber.getText().getIntValue());
}

void MainComponent::loadSettings()
{
	PropertiesFile::Options options;
	options.applicationName = "HTOSCBridgeSettings";
	options.filenameSuffix = ".conf";
	options.osxLibrarySubFolder = "Application Support";
	options.folderName = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getParentDirectory().getFullPathName();
	options.storageFormat = PropertiesFile::storeAsXML;
	appSettings.setStorageParameters(options);

	if (appSettings.getUserSettings()->getBoolValue("loadSettingsFile"))
	{
		//// osc
		//clientTxIpLabel.setText(appSettings.getUserSettings()->getValue("clientTxIp"), dontSendNotification);
		//clientTxPortLabel.setText(appSettings.getUserSettings()->getValue("clientTxPort"), dontSendNotification);
		//clientRxPortLabel.setText(appSettings.getUserSettings()->getValue("clientRxPort"), dontSendNotification);

		//// localisation component
		//m_localisationComponent.setAudioFilesDir(appSettings.getUserSettings()->getValue("locCompAudioFilesDir"));

		//// stimulus player
		//m_stimulusPlayer.setAudioFilesDir(appSettings.getUserSettings()->getValue("stimPlayerAudioFilesDir"));

		//// renderer view
		//m_rendererView.setCurrentTab(appSettings.getUserSettings()->getValue("rendererViewTabIndex"));

		//// router
		//m_lspkRouter.loadRoutingFile(appSettings.getUserSettings()->getValue("routingFile"));
		//m_lspkRouter.loadCalibrationFile(appSettings.getUserSettings()->getValue("calibrationFile"));
	}

	m_rollOscAddress.setText("/roll/", dontSendNotification);
	m_pitchOscAddress.setText("/pitch/", dontSendNotification);
	m_yawOscAddress.setText("/yaw/", dontSendNotification);
	m_rpyOscAddress.setText("/rendering/ypr/", dontSendNotification);

	m_rollOscMin.setText("180", dontSendNotification);
	m_pitchOscMin.setText("180", dontSendNotification);
	m_yawOscMin.setText("180", dontSendNotification);

	m_rollOscMax.setText("-180", dontSendNotification);
	m_pitchOscMax.setText("-180", dontSendNotification);
	m_yawOscMax.setText("-180", dontSendNotification);

	m_ipAddress.setText("127.0.0.1", dontSendNotification);
	m_portNumber.setText("9001", dontSendNotification);
	
	updateBridgeSettings();
}

void MainComponent::saveSettings()
{

	//// osc
	//appSettings.getUserSettings()->setValue("clientTxIp", clientTxIpLabel.getText());
	//appSettings.getUserSettings()->setValue("clientTxPort", clientTxPortLabel.getText());
	//appSettings.getUserSettings()->setValue("clientRxPort", clientRxPortLabel.getText());

	//// localisation component
	//appSettings.getUserSettings()->setValue("locCompAudioFilesDir", m_localisationComponent.getAudioFilesDir());

	//// stimulus player
	//appSettings.getUserSettings()->setValue("stimPlayerAudioFilesDir", m_stimulusPlayer.getAudioFilesDir());

	//// renderer view
	//appSettings.getUserSettings()->setValue("rendererViewTabIndex", m_rendererView.getCurrentTab());

	//// output router
	//appSettings.getUserSettings()->setValue("routingFile", m_lspkRouter.getRoutingFilePath());
	//appSettings.getUserSettings()->setValue("calibrationFile", m_lspkRouter.getCalibrationFilePath());

	appSettings.getUserSettings()->setValue("loadSettingsFile", true);
}