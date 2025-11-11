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

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Settings Window"

SettingsWindow::SettingsWindow(BRect frame)
	:
	BWindow(frame, B_TRANSLATE("Settings"), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	SetLayout(new BGroupLayout(B_VERTICAL));

	// === 1. Background color box ===
	BBox* backgroundBox = new BBox("BackgroundColorBox");
	backgroundBox->SetLabel(B_TRANSLATE("Background Color"));
	
	ColorButton* bgColorButton = new ColorButton("BGColorButton", 
		B_TRANSLATE("Background Color"), fBackgroundColor);
	BCheckBox* transparentCheckbox = new BCheckBox("TransparentCheckbox",
		B_TRANSLATE("Transparent background for replicant"), nullptr);

	BLayoutBuilder::Group<>(backgroundBox, B_VERTICAL, 5)
		.SetInsets(10)
		.Add(bgColorButton)
		.Add(transparentCheckbox);

	// === 2. Dot diameter field ===
	BTextControl* dotSize = new BTextControl("DotSize",
		B_TRANSLATE("Dot Diameter:"), "16", nullptr);

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
		.SetInsets(10)
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
		.SetInsets(10)
		.Add(fInactiveCenterButton)
		.Add(fInactiveEdgeButton);

	// === Main layout ===
	BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
		.SetInsets(10)
		.Add(backgroundBox)
		.Add(dotSize)
		.Add(infoLabel)
		.Add(activeBox)
		.Add(inactiveBox);

	Show();
}

SettingsWindow::~SettingsWindow() {
	
}

void SettingsWindow::MessageReceived(BMessage* message) {
	BWindow::MessageReceived(message);
}
