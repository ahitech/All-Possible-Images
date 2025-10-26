/*
 * Copyright 2025, Alex Hitech <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "SettingsWindow.h"

#include <View.h>
#include <Button.h>

SettingsWindow::SettingsWindow(BRect frame)
	: BWindow(frame, "Settings",
		B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	// Простой корневой вид
	BView* root = new BView(Bounds(), "settings_root", B_FOLLOW_ALL, B_WILL_DRAW);
	root->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(root);

	// Пока ничего не добавляем, окно будет пустым
	Show();
}

void SettingsWindow::MessageReceived(BMessage* message) {
	BWindow::MessageReceived(message);
}
