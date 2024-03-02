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
#include <juce_gui_basics/juce_gui_basics.h>

#include "BinaryData.h"
#include "BinauralHeadView.h"
#include "Bridge.h"
#include "SMLookAndFeel.h"

class MainComponent : public juce::Component,
                      public juce::Button::Listener,
                      private juce::Timer {
 public:
  MainComponent();
  ~MainComponent() override;

  SMLookAndFeel SMLF;

  void buttonClicked(juce::Button* buttonThatWasClicked) override;
  void timerCallback() override;
  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  void switchInput();
  void refreshPortList();
  void updateBridgeSettings();
  bool validateQuatsKey();
  void loadSettings();
  void saveSettings();
  void loadPreset(int index);
  bool loadPresetXml();

  juce::ApplicationProperties appSettings;
  juce::TextButton m_serialInputButton, m_oscInputButton;
  juce::TextButton m_refreshButton, m_connectButton, m_resetButton;
  juce::TextButton m_quatsOscActive, m_rollOscActive, m_pitchOscActive,
      m_yawOscActive, m_rpyOscActive;
  juce::ComboBox m_portListCB, m_yprOrderCB, m_oscPresetCB;
  juce::Label m_rollLabel, m_pitchLabel, m_yawLabel;
  juce::Label m_quatsKeyLabel;
  juce::Array<int> m_quatsOrder, m_quatsSigns;

  juce::Label m_quatsOscAddress, m_rollOscAddress, m_pitchOscAddress,
      m_yawOscAddress, m_rpyOscAddress;
  juce::Label m_rollOscMin, m_pitchOscMin, m_yawOscMin;
  juce::Label m_rollOscMax, m_pitchOscMax, m_yawOscMax;
  juce::Label m_rollOscVal, m_pitchOscVal, m_yawOscVal;
  juce::Label m_ipAddress, m_portNumber;

  Bridge bridge;

  // colors
  const juce::Colour clblue =
      juce::Colour::fromRGBA(101, 128, 200, 255);  // light blue
  const juce::Colour cgrey = juce::Colour::fromRGBA(62, 62, 62, 255);  //  grey
  const juce::Colour clrblue =
      juce::Colour::fromRGBA(209, 219, 244, 255);  // lighter blue
  const juce::Colour cdark = juce::Colour::fromRGBA(29, 29, 29, 255);  // dark
  const juce::Colour cgrnsh =
      juce::Colour::fromRGBA(101, 200, 158, 255);  // light greenish
  const juce::Colour cred = juce::Colour::fromRGBA(255, 0, 0, 255);  // red

  // fonts
  const juce::Font titlefontA =
      juce::Font(juce::Typeface::createSystemTypefaceFor(
          BinaryData::Tbold_ttf, BinaryData::Tbold_ttfSize));
  const juce::Font titlefontB =
      juce::Font(juce::Typeface::createSystemTypefaceFor(
          BinaryData::segoeui_ttf, BinaryData::segoeui_ttfSize));
  const juce::Font labelfont =
      juce::Font(juce::Typeface::createSystemTypefaceFor(
          BinaryData::segoeui_ttf, BinaryData::segoeui_ttfSize));

  // images
  juce::Image iserial = juce::ImageCache::getFromMemory(
      BinaryData::serial_png, BinaryData::serial_pngSize);
  juce::Image iaxis = juce::ImageCache::getFromMemory(BinaryData::axis_png,
                                                      BinaryData::axis_pngSize);
  juce::Image iosc = juce::ImageCache::getFromMemory(BinaryData::osc_png,
                                                     BinaryData::osc_pngSize);

  BinauralHeadView m_binauralHeadView;

  std::unique_ptr<juce::XmlElement> presetList;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
