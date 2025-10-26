#ifndef MATRIX_VIEW_H
#define MATRIX_VIEW_H

#include <Bitmap.h>
#include <View.h>
#include <Path.h>
#include <File.h>
#include <Window.h>


const int kRows = 8;
const int kCols = 8;
const int kDotSize = 16;
const int kDotSpacing = 20;
const bigtime_t kUpdateInterval = 100000; // 0.1s

class MatrixView : public BView {
public:
	MatrixView(BRect frame, const char* name);
	virtual ~MatrixView();

	void AttachedToWindow() override;
	void Draw(BRect updateRect) override;
	void MouseDown(BPoint where) override;
	void Pulse() override;

private:
	void LoadState();
	void SaveState();
	
	void InitDotBitmaps();
	void RenderDotGradient(BBitmap* bitmap, bool active);
	
	void InitBitPosSpiral();
	void DumpBitPos();

	uint8_t _index[32];       //!< Binary that is to be displayed
	uint8_t _gray[32];        //!< Binary Gray code of "_index"
	uint8_t _user_mask[32];   //!< User changes to the displayed matrix
	uint 	_bit_pos[8][8];   //!< Distribution of "_index" through the matrix

	
	int bit_pos[8][8];  // [y][x] = bitIndex
	
	BBitmap* _dotActive;		//!< Active dot in the matrix
	BBitmap* _dotInactive;		//!< Inactive dot in the matrix

	BPath _settingsPath;
};

#endif
