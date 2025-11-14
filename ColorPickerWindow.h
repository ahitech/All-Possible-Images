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


#define COLOR_SELECTED 'clrs'
#define COLOR_PICKER_OK 'cpok'
#define COLOR_PICKER_CANCEL 'cpcn'


class ColorPickerWindow : public BWindow {
public:
	ColorPickerWindow(BRect parentFrame,
			BMessenger target, 
			rgb_color initialColor);
	virtual void MessageReceived(BMessage* msg);

private:
	BColorControl* fColorControl;
	BMessenger fTarget;
};


#endif // COLOR_PICKER_WINDOW_H
