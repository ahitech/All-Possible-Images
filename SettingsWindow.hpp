/*
 * Copyright 2025, Alex Hitech <ahitech@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SETTINGS_WINDOW_HPP
#define SETTINGS_WINDOW_HPP


#include <SupportDefs.h>
#include <Window.h>

class SettingsWindow : public BWindow {
public:
	SettingsWindow(BRect frame);
	virtual void MessageReceived(BMessage* message);
};


#endif // SETTINGS_WINDOW_HPP
