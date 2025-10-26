#include "MatrixView.hpp"
#include <StorageDefs.h>
#include <FindDirectory.h>
#include <Entry.h>
#include <Directory.h>
#include <iostream>
#include <math.h> // For gradients
#include <string.h>

MatrixView::MatrixView(BRect frame, const char* name)
	: BView(frame, name, B_FOLLOW_NONE, B_WILL_DRAW | B_PULSE_NEEDED)
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

void MatrixView::InitDotBitmaps() {
	const int size = kDotSize;
	BRect bounds(0, 0, size - 1, size - 1);

	_dotActive = new BBitmap(bounds, B_RGBA32, true);
	_dotInactive = new BBitmap(bounds, B_RGBA32, true);

	RenderDotGradient(_dotActive, true);
	RenderDotGradient(_dotInactive, false);
}


void MatrixView::AttachedToWindow() {
	LoadState();

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
			int bitIndex = y * kCols + x;
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

void MatrixView::DrawCircle(int x, int y, bool active) {
	rgb_color color = active ? make_color(0, 0, 255) : make_color(64, 64, 64);
	SetHighColor(color);

	BRect r(x * kDotSpacing, y * kDotSpacing,
			x * kDotSpacing + kDotSize, y * kDotSpacing + kDotSize);
	FillEllipse(r);
}

void MatrixView::MouseDown(BPoint where) {
	int x = where.x / kDotSpacing;
	int y = where.y / kDotSpacing;
	if (x >= kCols || y >= kRows) return;

	int bitIndex = y * kCols + x;
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

	// DEBUGGING
    printf("Tick: index[0] = %02X, gray[0] = %02X\n", _index[0], _gray[0]);

    Invalidate();
}


void MatrixView::SaveState() {
	BFile file(_settingsPath.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return;

	file.Write(_index, sizeof(_index));
	file.Write(_user_mask, sizeof(_user_mask));
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

