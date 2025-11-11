/*
 * Copyright 2025, Alex Hitech <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SETTINGS_WINDOW_HPP
#define SETTINGS_WINDOW_HPP


#include <Window.h>
#include <View.h>
#include <Button.h>
#include <CheckBox.h>
#include <ColorControl.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <TextControl.h>
#include <StringView.h>
#include <Box.h>


class ColorButton;


class SettingsWindow : public BWindow {
public:
	SettingsWindow(BRect frame);
	virtual ~SettingsWindow();
	
	virtual void MessageReceived(BMessage* in);

protected:	
	ColorButton* fBgColorButton;
	BCheckBox* fTransparentCheck;
	rgb_color fBackgroundColor;
		
	BTextControl* fDotSizeText;
	uint fDotSize;
	
	ColorButton* fActiveCenterButton;
	ColorButton* fActiveEdgeButton;
	rgb_color fActiveCenterColor;
	rgb_color fActiveEdgeColor;
	
	ColorButton* fInactiveCenterButton;
	ColorButton* fInactiveEdgeButton;
	rgb_color fInactiveCenterColor;
	rgb_color fInactiveEdgeColor;
};

#endif // SETTINGS_WINDOW_HPP
