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
	SetLayout(new BGroupLayout(B_VERTICAL));
	
	fColorControl = new BColorControl(BPoint(10, 10),
		B_CELLS_16x16, 8.0f, "color_control");
	fColorControl->SetValue(initialColor);

	// Кнопки
	BButton* applyButton = new BButton(B_TRANSLATE("Apply"), new BMessage('aply'));
	BButton* cancelButton = new BButton(B_TRANSLATE("Cancel"), new BMessage('cncl'));

	// Выравнивание кнопок
	BGroupView* buttonRow = new BGroupView(B_HORIZONTAL);
	buttonRow->GroupLayout()->SetInsets(10, 10, 10, 10);
	buttonRow->GroupLayout()->AddView(cancelButton);
	buttonRow->GroupLayout()->AddView(applyButton);

	AddChild(fColorControl);
	AddChild(buttonRow);

	// Центрируем по экрану
//	CenterOnScreen();
}

void ColorPickerWindow::MessageReceived(BMessage* message) {
	switch (message->what) {
		case 'aply': {
			rgb_color selected = fColorControl->ValueAsColor();
				// TODO: Send a message to caller
			Quit();
			break;
		}
		case 'cncl': {
			Quit();
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}

