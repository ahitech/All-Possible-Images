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

const uint32	OK_MESSAGE		= 'Yes!';
const uint32	APPLY_MESSAGE	= 'Try ';
const uint32	CANCEL_MESSAGE	= 'No! ';
const uint32	DEFAULTS_MSG	= 'Back';
const uint32	TEXT_MODIFIED	= 'txtm';

class SettingsWindow : public BWindow {
public:
	SettingsWindow(BRect frame, BMessage& settings);
	virtual ~SettingsWindow() {};
	
	virtual void MessageReceived(BMessage* in);
	virtual void SetTarget(BView* in) { fTargetView = in; }
private:
	void ApplySettings();
	void ReturnToDefaults();
	
	ColorButton* fBgColorButton;
	BCheckBox*	fTransparentCheck;
	rgb_color	fBackgroundColor;
		
	BTextControl* fDotSizeText;
	uint fDotSize;
	
	ColorButton* fActiveCenterButton;
	ColorButton* fActiveEdgeButton;
	rgb_color 	fActiveCenterColor;
	rgb_color 	fActiveEdgeColor;
	
	ColorButton* fInactiveCenterButton;
	ColorButton* fInactiveEdgeButton;
	rgb_color 	fInactiveCenterColor;
	rgb_color	fInactiveEdgeColor;
	
	BButton 	*fOk, *fCancel, *fApply, *fDefaults;
	
	BMessage 	fSettingsMessage;
	BView*		fTargetView;
};

#endif // SETTINGS_WINDOW_HPP
