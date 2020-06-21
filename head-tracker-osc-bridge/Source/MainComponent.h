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
#include "Bridge.h"
#include "SMLookAndFeel.h"

class MainComponent   : public Component,
						public Button::Listener,
						private Timer
{
public:
    MainComponent();
    ~MainComponent();

	SMLookAndFeel SMLF;

	void buttonClicked(Button* buttonThatWasClicked) override;
	void timerCallback() override;
    void paint (Graphics&) override;
    void resized() override;

private:
	void refreshPortList();
	void updateBridgeSettings();
	bool validateQuatsKey();
	void loadSettings();
	void saveSettings();
	void loadPreset(int index);
	ApplicationProperties appSettings;

	TextButton m_refreshButton, m_connectButton, m_resetButton;
	TextButton m_quatsOscActive, m_rollOscActive, m_pitchOscActive, m_yawOscActive, m_rpyOscActive;
	ComboBox m_portListCB, m_yprOrderCB, m_oscPresetCB;
	Label m_rollLabel, m_pitchLabel, m_yawLabel;
	Label m_quatsKeyLabel;
	Array<int> m_quatsOrder, m_quatsSigns;

	Label m_quatsOscAddress, m_rollOscAddress, m_pitchOscAddress, m_yawOscAddress, m_rpyOscAddress;
	Label m_rollOscMin, m_pitchOscMin, m_yawOscMin;
	Label m_rollOscMax, m_pitchOscMax, m_yawOscMax;
	Label m_rollOscVal, m_pitchOscVal, m_yawOscVal;
	Label m_ipAddress, m_portNumber;
	
	Bridge bridge;

	// colors
	const Colour clblue = Colour::fromRGBA(101, 128, 200, 255); // light blue
	const Colour cgrey = Colour::fromRGBA(62, 62, 62, 255); //  grey
	const Colour clrblue = Colour::fromRGBA(209, 219, 244, 255); // lighter blue
	const Colour cdark = Colour::fromRGBA(29, 29, 29, 255); // dark
	const Colour cgrnsh = Colour::fromRGBA(101, 200, 158, 255); // light greenish
	const Colour cred = Colour::fromRGBA(255, 0, 0, 255); // red

	// fonts
	const Font titlefontA = Font(Typeface::createSystemTypefaceFor(BinaryData::Tbold_ttf, BinaryData::Tbold_ttfSize));
	const Font titlefontB = Font(Typeface::createSystemTypefaceFor(BinaryData::segoeui_ttf, BinaryData::segoeui_ttfSize));
	const Font labelfont = Font(Typeface::createSystemTypefaceFor(BinaryData::segoeui_ttf, BinaryData::segoeui_ttfSize));

	// images
	Image iserial = ImageCache::getFromMemory(BinaryData::serial_png, BinaryData::serial_pngSize);
	Image iaxis = ImageCache::getFromMemory(BinaryData::axis_png, BinaryData::axis_pngSize);
	Image iosc = ImageCache::getFromMemory(BinaryData::osc_png, BinaryData::osc_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
