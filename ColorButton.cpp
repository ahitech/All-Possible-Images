/*
 * Copyright 2025, Alexey Burshtein <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "ColorButton.h"
#include "ColorPickerWindow.h"

#include <Alert.h>
#include <ColorControl.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <Window.h>
#include <TranslationUtils.h>
#include <Catalog.h>
#include <Bitmap.h>

#include <stdio.h>

ColorButton::ColorButton(const char* name, const char* label,
                         rgb_color initialColor, BMessage* toSend)
    : BButton(name,
    			label,
    			toSend,
    			B_FOLLOW_LEFT | B_FOLLOW_TOP),
      fColor(initialColor)
{
    SetViewColor(B_TRANSPARENT_COLOR);
//    SetTarget(fTarget);
}

void ColorButton::SetColor(rgb_color color) {
    fColor = color;
    Invalidate();
}

void ColorButton::Draw(BRect updateRect) {
	BButton::Draw(updateRect);

	// Draw the color rectangle inside the button
	BRect rect = Bounds();
	rect.InsetBy(6, 6);
	rgb_color fillColor = fColor;

	SetHighColor(fillColor);
	FillRect(rect);
		
	SetHighColor(0, 0, 0);
	StrokeRect(rect);
	
	// Draw label text over the colored area
	font_height fh;
	GetFontHeight(&fh);
	float textHeight = fh.ascent + fh.descent;
	float textY = rect.top + (static_cast<int>(rect.Height()) - textHeight) / 2 + fh.ascent;
	float textX = rect.left + 4;
		
	// Decide on text color based on fill color brightness
	rgb_color fill = fColor;
	float luminance = 0.299f * fill.red + 0.587f * fill.green + 0.114f * fill.blue;
	rgb_color textColor = (luminance < 128) ? make_color(255, 255, 255) : make_color(0, 0, 0);
	
	SetHighColor(textColor);
	DrawString(Label(), BPoint(textX, textY));
}

void ColorButton::MouseDown(BPoint where) {
	if (Window() == nullptr)
		return;
	
	BRect screenFrame = Window()->Frame();
	BRect windowRect(0, 0, 250, 200);
	BPoint center = screenFrame.LeftTop();
	center.x += screenFrame.Width() / 2;
	center.y += screenFrame.Height() / 2;
	windowRect.OffsetTo(center.x - windowRect.Width() / 2, center.y - windowRect.Height() / 2);
		
	BMessenger messenger(this);
	ColorPickerWindow* picker = new ColorPickerWindow(windowRect,
		messenger,
		fColor);
	picker->Show();
}

void ColorButton::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case 'colr':
		{
			rgb_color *newColor;
			size_t readSize;
			if (B_OK == message->FindData("color",
							B_RGB_COLOR_TYPE,
							&newColor,
							&readSize) &&
				newColor &&
				readSize == sizeof(rgb_color))
			{
				SetColor(*newColor);
			}
			
			break;
		}
		default:
			BButton::MessageReceived(message);
			break;
	};
}
