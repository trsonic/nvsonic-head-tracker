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

class SMLookAndFeel : public LookAndFeel_V3
{
public:
	SMLookAndFeel()
	{
		setColour(Slider::rotarySliderFillColourId, Colours::red);
	}

	const Colour clblue = Colour::fromRGBA(101, 128, 200, 255); // light blue
	const Colour cgrey = Colour::fromRGBA(62, 62, 62, 255); //  grey
	const Colour clrblue = Colour::fromRGBA(209, 219, 244, 255); // lighter blue
	const Colour cdark = Colour::fromRGBA(29, 29, 29, 255); // dark
	const Font buttonfont = Font(Typeface::createSystemTypefaceFor(BinaryData::segoeui_ttf, BinaryData::segoeui_ttfSize));

	void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
		bool isMouseOverButton, bool isButtonDown) override
	{
		Rectangle<int> buttonArea = button.getLocalBounds();
		if (isMouseOverButton == false) {
			g.setColour(backgroundColour);
		}
		else {
			if (isButtonDown == false) {
				g.setColour(clrblue);
			}
			else {
				g.setColour(backgroundColour);
			}

		}

		g.fillRoundedRectangle(buttonArea.toFloat(), 3.0f);
	}

	void drawButtonText(Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
	{
		g.setFont(buttonfont);
		g.setFont(20);
		g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
			: TextButton::textColourOffId)
			.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

		const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
		const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

		const int fontHeight = roundToInt(buttonfont.getHeight() * 0.6f);
		const int leftIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
		const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
		const int textWidth = button.getWidth() - leftIndent - rightIndent;

		if (textWidth > 0)
			g.drawFittedText(button.getButtonText(),
				leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
				Justification::centred, 2);
	}

	void drawComboBox(Graphics& g, int width, int height, const bool /*isButtonDown*/,
		int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box) override
	{
		g.setColour(clrblue);
		g.fillRoundedRectangle(0, 0, width, height, 3.0f);

		//g.fillAll(clrblue);
		// const Colour buttonColour(box.findColour(ComboBox::buttonColourId));

		//if (box.isEnabled() && box.hasKeyboardFocus(false))
		//{
		//	g.setColour(clrblue);
		//	g.fillRoundedRectangle(0, 0, width, height, 3.0f);
		//}
		//else
		//{
		//	g.setColour(clblue);
		//	//g.drawRect(0, 0, width, height);
		//	g.fillRoundedRectangle(0, 0, width, height, 3.0f);
		//}

		const float arrowX = 0.3f;
		const float arrowH = 0.2f;

		Path p;
		p.addTriangle(buttonX + buttonW * 0.5f, buttonY + buttonH * (0.45f - arrowH),
			buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
			buttonX + buttonW * arrowX, buttonY + buttonH * 0.45f);

		p.addTriangle(buttonX + buttonW * 0.5f, buttonY + buttonH * (0.55f + arrowH),
			buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
			buttonX + buttonW * arrowX, buttonY + buttonH * 0.55f);

		g.setColour(box.findColour(ComboBox::arrowColourId).withMultipliedAlpha(box.isEnabled() ? 1.0f : 0.3f));
		g.fillPath(p);
	}

	void drawLabel(Graphics& g, Label& label) override
	{
		//g.fillAll(label.findColour(Label::backgroundColourId));
		g.setColour(label.findColour(Label::backgroundColourId));
		g.fillRoundedRectangle(label.getLocalBounds().toFloat(), 3.0f);

		if (!label.isBeingEdited())
		{
			auto alpha = label.isEnabled() ? 1.0f : 0.5f;
			const Font font(getLabelFont(label));

			g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
			g.setFont(font);

			auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

			g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
				jmax(1, (int)(textArea.getHeight() / font.getHeight())),
				label.getMinimumHorizontalScale());

			g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
		}
		else if (label.isEnabled())
		{
			g.setColour(label.findColour(Label::outlineColourId));
		}

		//g.drawRect(label.getLocalBounds());
	}
};
