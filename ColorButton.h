/*
 * Copyright 2025, Alexey Burshtein <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H


#include <SupportDefs.h>

#include <Button.h>
#include <ColorControl.h>
#include <Window.h>
#include <Messenger.h>
#include <Message.h>
#include <Rect.h>
#include <String.h>
#include <View.h>

class ColorButton : public BButton {
public:
	ColorButton(const char* name,
			const char* label,
			rgb_color initialColor,
			BMessage* message = nullptr);
	
	virtual void Draw(BRect updateRect) override;
	virtual void MouseDown(BPoint where) override;
	virtual void MessageReceived(BMessage* in) override;
	rgb_color Color() const { return fColor; }
	void SetColor(rgb_color color);

private:
	rgb_color fColor;
	BString fLabel;
};

#endif // COLOR_BUTTON_H
