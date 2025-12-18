/*
 * Copyright 2025, Alexey Burshtein <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef COLOR_PICKER_WINDOW_H
#define COLOR_PICKER_WINDOW_H

#include <Window.h>
#include <ColorControl.h>
#include <Button.h>
#include <Messenger.h>

const uint32 COLOR_SELECTED			= 'clrs';
const uint32 COLOR_PICKER_OK		= 'cpok';
const uint32 COLOR_PICKER_CANCEL	= 'cpcn';
const uint32 SEND_COLOR_TO_BUTTON	= 'colr';

class ColorPreviewView : public BView {
public:
	ColorPreviewView();	
	inline void SetColor(rgb_color color) {
		fColor = color;
		Invalidate();
	}
	inline rgb_color Color() const { return fColor; }
	void Draw(BRect) override;
private:
	rgb_color fColor;
};

class ColorPickerWindow : public BWindow {
public:
	ColorPickerWindow(BRect parentFrame,
			BMessenger target, 
			rgb_color initialColor);
	virtual void MessageReceived(BMessage* msg);

private:
	BColorControl* fColorControl;
	ColorPreviewView*	fPreview; 
	BMessenger fTarget;
};


#endif // COLOR_PICKER_WINDOW_H
