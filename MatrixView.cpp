#include "MatrixView.hpp"
#include "SettingsWindow.hpp"

#include <StorageDefs.h>
#include <Catalog.h>
#include <LocaleRoster.h>
#include <FindDirectory.h>
#include <Entry.h>
#include <Directory.h>
#include <PopUpMenu.h>
#include <Screen.h>
#include <MenuItem.h>
#include <Alert.h>

#include <iostream>
#include <math.h> // For gradients
#include <string.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MatrixView"

MatrixView::MatrixView(BRect frame, const char* name)
	: BView(frame, name, B_FOLLOW_NONE, B_WILL_DRAW | B_PULSE_NEEDED),
	winPos(100, 100)		// Default window position
{
	memset(_index, 0, sizeof(_index));
	memset(_gray, 0, sizeof(_gray));
	memset(_user_mask, 0, sizeof(_user_mask));
	
	// Clearing the bitmaps before usage
	_dotActive = nullptr;
	_dotInactive = nullptr;
	// Prepare the bitmaps
	InitDotBitmaps();

	find_directory(B_USER_SETTINGS_DIRECTORY, &_settingsPath);
	_settingsPath.Append("AllPossibleImages.dat");

	SetViewColor(B_TRANSPARENT_COLOR);
	SetLowColor(make_color(0, 0, 0));
}

MatrixView::~MatrixView() {
	SaveState();
	if (_dotActive) delete _dotActive;
	if (_dotInactive) delete _dotInactive;
}


void MatrixView::MessageReceived(BMessage* in) {	
	switch (in->what) {
		case OPEN_PREFERENCES:		
		{
			// DEBUGGING
			printf ("Trying to show the settings...\n");
			_ShowSettingsWindow();
			break;
		}
		case B_ABOUT_REQUESTED:
		{
			BAlert* toShow = new BAlert(B_TRANSLATE("About the program"),
						B_TRANSLATE("All Possible Images for Haiku OS\n"
						"By Alexey \"Hitech\" Burshtein\nVersion 1.0\n\n"
						"Inspired by the \"allPossibleImages\" program for BeOS R3 "
						"by Douglas Irving Repetto"),
						B_TRANSLATE("OK"));
			if (toShow) {
//				toShow->MoveTo(100, 100);
				toShow->Go(NULL);
			}
			break;
		}
		default:
			BView::MessageReceived(in);
	};
}

void MatrixView::InitDotBitmaps() {
	const int size = kDotSize;
	BRect bounds(0, 0, size - 1, size - 1);

	_dotActive = new BBitmap(bounds, B_RGBA32, true);
	_dotInactive = new BBitmap(bounds, B_RGBA32, true);

	RenderDotGradient(_dotActive, true);
	RenderDotGradient(_dotInactive, false);
}


void MatrixView::AttachedToWindow() {
	BView::AttachedToWindow();
	LoadState();
	InitBitPosSpiral();

	// Setting Pulse() frequency
	if (Window())
		Window()->SetPulseRate(kUpdateInterval);
}

void MatrixView::Draw(BRect) {
	SetDrawingMode(B_OP_COPY);
	SetHighColor(0, 0, 0, 0);
	FillRect(Bounds());			// Clearing the background
	
	SetDrawingMode(B_OP_ALPHA); // Using alpha channels for transparency

	for (int y = 0; y < kRows; ++y) {
		for (int x = 0; x < kCols; ++x) {
			int bitIndex = bit_pos[x][y];

			int byteIndex = bitIndex / 8;
			int bitOffset = bitIndex % 8;

			bool gray = (_gray[byteIndex] >> bitOffset) & 1;
			bool user = (_user_mask[byteIndex] >> bitOffset) & 1;
			bool visible = gray ^ user;

			BBitmap* dot = visible ? _dotActive : _dotInactive;
			BPoint topLeft(x * kDotSpacing + (kDotSpacing - kDotSize)/2,
						   y * kDotSpacing + (kDotSpacing - kDotSize)/2);

			// Output
			DrawBitmap(dot, topLeft);
		}
	}
}

void MatrixView::MouseDown(BPoint where) {
	
	// Maybe I should open the context menu instead?..
	BMessage* message = Window()->CurrentMessage();
	int32 buttons;
	if (message->FindInt32("buttons", &buttons) == B_OK) {
		if (buttons & B_SECONDARY_MOUSE_BUTTON) {
			_ShowContextMenu(where);
			return;
		}
	}
	
	// Nah, it wasn't a right click. Let's continue with the boring stuff...
	int x = where.x / kDotSpacing;
	int y = where.y / kDotSpacing;
	if (x >= kCols || y >= kRows) return;

	int bitIndex = bit_pos[x][y];

	int byteIndex = bitIndex / 8;
	int bitOffset = bitIndex % 8;

	_user_mask[byteIndex] ^= (1 << bitOffset);

	Invalidate();
}

void MatrixView::Pulse() {
    // Increasing the binary number to be shown
    for (int i = 0; i < 32; ++i) {
        if (++_index[i] != 0)
            break;
    }

    // Calculating the Gray code = index ^ (index >> 1)
    uint8_t carry = 0;
    for (int i = 31; i >= 0; --i) {
        uint8_t shifted = (_index[i] >> 1) | (carry << 7);
        _gray[i] = _index[i] ^ shifted;
        carry = _index[i] & 1;
    }

    Invalidate();
}

void MatrixView::SaveState() {
	BFile file(_settingsPath.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return;

	file.Write(_index, sizeof(_index));
	file.Write(_user_mask, sizeof(_user_mask));
	
	// Save window position
	file.Write(&winPos, sizeof(BPoint));
}

void MatrixView::LoadState() {
	BFile file(_settingsPath.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	ssize_t read1 = file.Read(_index, sizeof(_index));
	ssize_t read2 = file.Read(_user_mask, sizeof(_user_mask));
	if (read1 != sizeof(_index))
		memset(_index, 0, sizeof(_index));
	if (read2 != sizeof(_user_mask))
		memset(_user_mask, 0, sizeof(_user_mask));

	// Translation from _index to _gray
	_gray[0] = _index[0];
	for (int i = 1; i < 32; ++i)
		_gray[i] = _index[i] ^ _index[i - 1];
		
	// Restore window position
	BPoint winPos;
	ssize_t readPos = file.Read(&winPos, sizeof(BPoint));
	if (readPos == sizeof(BPoint) && Window()) {		
		BScreen screen(Window());
		BRect screenFrame = screen.Frame();

		// If the window occurs outside of the screen view area,
		// (for example, because the resolution has changed),
		if (screenFrame.Contains(winPos))
			Window()->MoveTo(winPos);
		else
			Window()->MoveTo(100, 100); // Move the window to origin
	}
}

void MatrixView::RenderDotGradient(BBitmap* bitmap, bool active) {
	uint8* bits = (uint8*)bitmap->Bits();
	int32 bpr = bitmap->BytesPerRow();
	const int size = kDotSize;
	const int radius = kDotSize / 2;


	rgb_color center = active ? make_color(60, 60, 255) : make_color(0, 0, 0);
	rgb_color edge   = active ? make_color(0, 0, 120) : make_color(64, 64, 64);

	
	for (int y = 0; y < size; ++y) {
		for (int x = 0; x < size; ++x) {
			float dx = x - radius + 0.5f;
			float dy = y - radius + 0.5f;
			float dist = sqrtf(dx * dx + dy * dy);
			uint8* px = bits + y * bpr + x * 4;

			if (dist > radius) {
				px[0] = px[1] = px[2] = px[3] = 0; // Transparency
				continue;
			}

			float t = std::min(dist / radius, 0.999f);
			float f = log10f(10.0f - 9.0f * t) / log10f(10.0f);

			uint8 r = (uint8)(center.red   + (edge.red   - center.red)   * (1.0f - f));
			uint8 g = (uint8)(center.green + (edge.green - center.green) * (1.0f - f));
			uint8 b = (uint8)(center.blue  + (edge.blue  - center.blue)  * (1.0f - f));

			px[0] = b;   // blue
			px[1] = g;   // green
			px[2] = r;   // red
			px[3] = 255; // alpha
		}
	}
}

void MatrixView::InitBitPosSpiral() {
	for (int x = 0; x < kCols; ++x)
		for (int y = 0; y < kRows; ++y)
			bit_pos[x][y] = -1;

	int x = 4, y = 3;           // Starting point
	
	// Clockwise spiral defining steps:
	int dx[] = { 0, -1, 0, 1 };	// Stop, left, stop, right
	int dy[] = { 1, 0, -1, 0 };	// Down, stop,   up, stop

	int dir = 0;
	int steps = 1;
	int bit = 0;

	while (bit < 64) {
		for (int side = 0; side < 2; ++side) {
			for (int i = 0; i < steps; ++i) {
				if (x >= 0 && x < kCols && 
					y >= 0 && y < kRows && 
					bit_pos[x][y] == -1)
				{
					bit_pos[x][y] = bit++;
					if (bit >= kRows * kCols) break;
				}
				x += dx[dir];
				y += dy[dir];
			}
			dir = (dir + 1) % 4;
		}
		steps++;
	}

// 	DumpBitPos(); // DEBUGGING - just a beautiful printout of the matrix :)
}

void MatrixView::DumpBitPos() {
	printf("Bit position matrix (bit_pos[x][y]):\n");

	for (int y = 0; y < kRows; ++y) {
		for (int x = 0; x < kCols; ++x) {
			if (bit_pos[x][y] >= 0)
				printf("%3d ", bit_pos[x][y]);
			else
				printf("  . ");
		}
		printf("\n");
	}
}

void MatrixView::_ShowContextMenu(BPoint point)
{
	ConvertToScreen(&point);

	BPopUpMenu* menu = new BPopUpMenu("context_menu", false, false);
	menu->SetAsyncAutoDestruct(true);

	BMenuItem* settingsItem = new BMenuItem(B_TRANSLATE("⚙ Settings…"), 
	                                        new BMessage(OPEN_PREFERENCES));
	settingsItem->SetTarget(this);
	menu->AddItem(settingsItem);
	
	BMenuItem* aboutItem = new BMenuItem(B_TRANSLATE("ℹ About"), 
	                                     new BMessage(B_ABOUT_REQUESTED));
	aboutItem->SetTarget(this);
	menu->AddItem(aboutItem);

	menu->Go(point, true, true, BRect(point - BPoint(4, 4), point + BPoint(4, 4)), true);
}


void MatrixView::_ShowSettingsWindow() {
	BRect screenFrame = BScreen().Frame();
	BRect winFrame(100, 100, 400, 300);

	// Центрируем окно
	winFrame.OffsetTo(
		(screenFrame.Width() - winFrame.Width()) / 2,
		(screenFrame.Height() - winFrame.Height()) / 2
	);

	new SettingsWindow(winFrame); // Само себя покажет
}
