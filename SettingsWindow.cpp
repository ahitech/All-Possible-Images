/*
 * Copyright 2025, Alex Hitech <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "SettingsWindow.h"
#include "ColorButton.h"

#include <View.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <ColorControl.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <StringView.h>
#include <TextControl.h>
#include <TranslationUtils.h>
#include <LocaleRoster.h>
#include <Catalog.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>

const uint		kMinDotSize 	= 5;
const uint		kMaxDotSize		= 100;

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Settings Window"

SettingsWindow::SettingsWindow(BRect frame, BMessage& settings)
	:
	BWindow(frame, B_TRANSLATE("Settings"), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fSettingsMessage(settings),
	fOk(nullptr), fApply(nullptr), fCancel(nullptr)
{
	
	SetLayout(new BGroupLayout(B_VERTICAL));

	// === 1. Background color box ===
	BBox* backgroundBox = new BBox("BackgroundColorBox");
	backgroundBox->SetLabel(B_TRANSLATE("Background Color"));
	
	fBgColorButton = new ColorButton("BGColorButton", 
		B_TRANSLATE("Background Color"), fBackgroundColor);
	fTransparentCheck = new BCheckBox("TransparentCheckbox",
		B_TRANSLATE("Transparent background for replicant"), nullptr);
	fTransparentCheck->SetValue(true);

	BLayoutBuilder::Group<>(backgroundBox, B_VERTICAL, 5)
		.SetInsets(15, 20, 15, 5)
		.Add(fBgColorButton)
		.Add(fTransparentCheck);

	// === 2. Dot diameter field ===
	fDotSizeText = new BTextControl("DotSize",
		B_TRANSLATE("Dot Diameter:"), "16", nullptr);
	fDotSizeText->SetModificationMessage(new BMessage(TEXT_MODIFIED));
	fDotSizeText->SetTarget(this);

	// === 3. Info string ===
	BStringView* infoLabel = new BStringView("InfoLabel",
		B_TRANSLATE("Window size depends on dot diameter."));

	// === 4. Active dot color box ===
	BBox* activeBox = new BBox("ActiveDotColorBox");
	activeBox->SetLabel("Active Dot Color");

	fActiveCenterButton = new ColorButton("ActiveCenter",
		B_TRANSLATE("Center Color"), fActiveCenterColor);
	fActiveEdgeButton = new ColorButton("ActiveEdge",
		B_TRANSLATE("Edge Color"), fActiveEdgeColor);

	BLayoutBuilder::Group<>(activeBox, B_VERTICAL, 5)
		.SetInsets(15, 20, 15, 10)
		.Add(fActiveCenterButton)
		.Add(fActiveEdgeButton);

	// === 5. Inactive dot color box ===
	BBox* inactiveBox = new BBox("InactiveDotColorBox");
	inactiveBox->SetLabel("Inactive Dot Color");

	fInactiveCenterButton = new ColorButton("InactiveCenter",
		B_TRANSLATE("Center Color"), fInactiveCenterColor);
	fInactiveEdgeButton = new ColorButton("InactiveEdge",
		B_TRANSLATE("Edge Color"), fInactiveEdgeColor);

	BLayoutBuilder::Group<>(inactiveBox, B_VERTICAL, 5)
		.SetInsets(15, 20, 15, 10)
		.Add(fInactiveCenterButton)
		.Add(fInactiveEdgeButton);
		
	// === 6. Action buttons ===
	fCancel	= new BButton("cancel", B_TRANSLATE("❌ Cancel"),
						new BMessage(CANCEL_MESSAGE));
	fApply	= new BButton("apply", B_TRANSLATE("☑️ Apply"),
						new BMessage(APPLY_MESSAGE));
	fOk 	= new BButton("ok", B_TRANSLATE("🆗 OK"), 
						new BMessage(OK_MESSAGE));
	fDefaults = new BButton("defaults", B_TRANSLATE("🔄 Return to defaults"),
						new BMessage(DEFAULTS_MSG));
	fOk->MakeDefault(true);
	fCancel->SetTarget(this);
	fApply->SetTarget(this);
	fOk->SetTarget(this);
	fDefaults->SetTarget(this);
	BGroupView* buttonRow = new BGroupView(B_HORIZONTAL);
	buttonRow->GroupLayout()->SetInsets(0, 0, 0, 0); // Margins
	buttonRow->GroupLayout()->AddView(fCancel);
	buttonRow->GroupLayout()->AddView(fApply);
	buttonRow->GroupLayout()->AddView(fOk);

	// === Main layout ===
	BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
		.SetInsets(10)
		.Add(backgroundBox)
		.Add(fDotSizeText)
		.Add(infoLabel)
		.Add(activeBox)
		.Add(inactiveBox)
		.Add(fDefaults)
		.Add(buttonRow);

	Show();
}

void SettingsWindow::ReturnToDefaults() {
	fActiveCenterColor = make_color(60, 60, 255);
	fActiveCenterButton->SetColor(fActiveCenterColor);
	fInactiveCenterColor = make_color(0, 0, 0);
	fInactiveCenterButton->SetColor(fInactiveCenterColor);
	fActiveEdgeColor = make_color(0, 0, 120);
	fActiveEdgeButton->SetColor(fActiveEdgeColor);
	fInactiveEdgeColor = make_color(64, 64, 64);
	fInactiveEdgeButton->SetColor(fInactiveEdgeColor);
	fTransparentCheck->SetValue(true);
	fBackgroundColor = make_color(0, 0, 0);
	fBgColorButton->SetColor(fBackgroundColor);
	fDotSizeText->SetText("16");
	UpdateIfNeeded();
}

void SettingsWindow::MessageReceived(BMessage* message) {
	switch (message->what) {
		case TEXT_MODIFIED:
		{
			char buffer[8];
			strncpy(buffer, fDotSizeText->Text(), 8);
			char *endptr = nullptr;
			
			long parsed = strtol(buffer, &endptr, 0);
			uint previousDotSize = fDotSize;
			
			if (parsed < kMinDotSize) {
				fDotSize = kMinDotSize;
			} else if (parsed > kMaxDotSize) {
				fDotSize = kMaxDotSize;
			} else {
				fDotSize = static_cast<int>(parsed);
			}
			
			if (previousDotSize != fDotSize) {
				snprintf(buffer, sizeof(buffer), "%ld", fDotSize);					
				fDotSizeText->SetText(buffer);
				fDotSizeText->TextView()->Select(3, 3);
			}
			break;
		}
		case OK_MESSAGE:
			ApplySettings();
			Quit(); // Close window after applying settings
			break;

		case APPLY_MESSAGE:
			ApplySettings(); // Just applying the settings
			break;

		case CANCEL_MESSAGE:
			Quit(); // Just closing the window
			break;
		
		case DEFAULTS_MSG:
			ReturnToDefaults();
			break;

		default:
			break;
	}
	BWindow::MessageReceived(message);
}

void SettingsWindow::ApplySettings()
{
	fActiveCenterColor = fActiveCenterButton->Color();
	fSettingsMessage.ReplaceData("active_center", B_RGB_COLOR_TYPE,
		&fActiveCenterColor, sizeof(rgb_color));
	fActiveEdgeColor = fActiveEdgeButton->Color();
	fSettingsMessage.ReplaceData("active_edge", B_RGB_COLOR_TYPE,
		&fActiveEdgeColor, sizeof(rgb_color));
	fInactiveCenterColor = fInactiveCenterButton->Color();
	fSettingsMessage.ReplaceData("inactive_center", B_RGB_COLOR_TYPE,
		&fInactiveCenterColor, sizeof(rgb_color));
	fInactiveEdgeColor = fInactiveEdgeButton->Color();
	fSettingsMessage.ReplaceData("inactive_edge", B_RGB_COLOR_TYPE,
		&fInactiveEdgeColor, sizeof(rgb_color));
	fBackgroundColor = fBgColorButton->Color();
	fSettingsMessage.ReplaceData("background", B_RGB_COLOR_TYPE,
		&fBackgroundColor, sizeof(rgb_color));
	
	fSettingsMessage.ReplaceInt32("dot_size", fDotSize);

	bool transparent = (fTransparentCheck->Value() == B_CONTROL_ON);
	fSettingsMessage.ReplaceBool("is_transparent", transparent);

	if (fTargetView &&
		fTargetView->Window() &&
		fTargetView->Window()->LockLooper())
	{
//		fTargetView->PostMessage(&fSettingsMessage);
		fTargetView->Window()->UnlockLooper();
	}
}
