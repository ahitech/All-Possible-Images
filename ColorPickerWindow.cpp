/*
 * Copyright 2025, Alexey Burshtein <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

// ColorPickerWindow.cpp
#include "ColorPickerWindow.h"

#include <LayoutBuilder.h>
#include <Screen.h>
#include <Button.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <TranslationUtils.h>
#include <Catalog.h>
#include <ColorControl.h>
#include <stdio.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Color Preview View"

ColorPreviewView::ColorPreviewView()
	:
	BView("output_preview", B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
{
	SetViewColor(B_TRANSPARENT_COLOR);
	fColor = make_color(128, 128, 128); // default color
}

void ColorPreviewView::Draw(BRect) {
	BRect bounds = Bounds();

	SetHighColor(fColor);
	FillRect(bounds);

	SetHighColor(0, 0, 0);
	StrokeRect(bounds);

	uint16 brightness = fColor.red * 30 + fColor.green * 59 + fColor.blue * 11;
	rgb_color textColor = (brightness > 12800) ? make_color(0, 0, 0) : make_color(255, 255, 255);

	SetHighColor(textColor);
	SetFont(be_bold_font);

	const char* label = B_TRANSLATE("Output color");
	font_height fh;
	GetFontHeight(&fh);

	float textWidth = StringWidth(label);
	float x = (bounds.Width() - textWidth) / 2.0f;
	float y = (bounds.Height() + fh.ascent - fh.descent) / 2.0f;

	DrawString(label, BPoint(x, y));
}


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Color Picker Window"

ColorPickerWindow::ColorPickerWindow(BRect frame, 
									BMessenger target,
									rgb_color initialColor)
	: BWindow(frame, B_TRANSLATE("Choose Color"),
				B_BORDERED_WINDOW_LOOK | 
				B_MODAL_APP_WINDOW_FEEL,
				B_NOT_ZOOMABLE | B_NOT_RESIZABLE),
		fTarget(target)
{
	
	fColorControl = new BColorControl(BPoint(10, 10),
		B_CELLS_16x16, 8.0f, "color_control");
	fColorControl->SetValue(initialColor);
	fColorControl->SetMessage(new BMessage(B_VALUE_CHANGED));
	fColorControl->SetTarget(this);
	
	fPreview = new ColorPreviewView();
	fPreview->SetExplicitMinSize(BSize(frame.Width() / 2, 22));
	fPreview->SetColor(initialColor);

	// Buttons
	BButton* applyButton = new BButton(B_TRANSLATE("Apply"), new BMessage('aply'));
	BButton* cancelButton = new BButton(B_TRANSLATE("Cancel"), new BMessage('cncl'));

	// Buttons alignment
	BGroupView* buttonRow = new BGroupView(B_HORIZONTAL);
	buttonRow->GroupLayout()->SetInsets(10, 0, 10, 0);
	buttonRow->GroupLayout()->AddView(cancelButton);
	buttonRow->GroupLayout()->AddView(applyButton);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
		.SetInsets(10)
		.Add(fColorControl)
		.Add(fPreview)
		.Add(buttonRow);
}

void ColorPickerWindow::MessageReceived(BMessage* message) {
	switch (message->what) {
		case 'aply': {
			rgb_color selected = fColorControl->ValueAsColor();
			BMessage* toSend = new BMessage(SEND_COLOR_TO_BUTTON);
			toSend->AddData("color",
						B_RGB_COLOR_TYPE,
						&selected,
						sizeof(rgb_color));
			fTarget.SendMessage(toSend);
			Quit();
			break;
		}
		case 'cncl': {
			Quit();
			break;
		}
		case B_VALUE_CHANGED: {
			rgb_color color = fColorControl->ValueAsColor();
			fPreview->SetColor(color);
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}

