#ifndef MATRIX_VIEW_H
#define MATRIX_VIEW_H

#include <Archivable.h>
#include <Bitmap.h>
#include <Dragger.h>
#include <File.h>
#include <Path.h>
#include <View.h>
#include <Window.h>
#include <iostream>

const int kRows = 8;
const int kCols = 8;
const int kDotSize = 16;
const int kDotSpacing = 20;
const uint	kDraggerSize = 7; 	// By default, BDragger is 7x7 pixels 
const bigtime_t kUpdateInterval = 100000; // 0.1s

//! The following #define, if not commented out, allows the program
//	to perform debugging printouts into a file in the user ~ directory.
// 	Keep it commented out to prevent unnecessary disk access.
//  #define _DEBUG_PRINTOUTS

const uint32	OPEN_PREFERENCES = 'pref';
const uint32	MESSAGE_RELEASED = 'asdb';

class MatrixView : public BView {
public:
	MatrixView(BRect frame, const char* name);
	MatrixView(BMessage* archive, bool isReplicant = false);
	virtual ~MatrixView();
	
	// Replication
	static BArchivable* Instantiate(BMessage* archive);
	virtual status_t Archive(BMessage* archive, bool deep = true) const override;
	
	void AttachedToWindow() override;
	void Draw(BRect updateRect) override;
	void MouseDown(BPoint where) override;
	void Pulse() override;
	void MessageReceived(BMessage* in) override;
	
	void SaveState();	// When application quits, the StateSave() function is called first
	bool IsReplicant() const {
		return this->_isReplicant;
	}
	void SaveNewPosition(BPoint point);

private:
	void LoadState();
	
	void InitDotBitmaps();
	void RenderDotGradient(BBitmap* bitmap, bool active);
	
	void _ShowContextMenu(BPoint point);
	void _ShowSettingsWindow();

	
	// This code creates the clockwise spiral from the center outwards.
	// The original program displayed the LSB of the number at dot (5, 4),
	// and the bits grew outwards in the following pattern:
	//	Bit position matrix (bit_pos[x][y]):
	// 		63  36  37  38  39  40  41  42 
	// 		62  35  16  17  18  19  20  43 
	// 		61  34  15   4   5   6  21  44 
	// 		60  33  14   3   0   7  22  45 
	// 		59  32  13   2   1   8  23  46 
	// 		58  31  12  11  10   9  24  47 
	// 		57  30  29  28  27  26  25  48 
	// 		56  55  54  53  52  51  50  49 
	// This matrix is the _bit_pos variable.
	uint _bit_pos[kRows][kCols];   //!< Distribution of "_index" through the matrix
	void InitBitPosSpiral();	//!< Initialize the _bit_pos matrix
	void DumpBitPos();			//!< Debugging output function

	uint8_t _index[32];       //!< Binary that is to be displayed
	uint8_t _gray[32];        //!< Binary Gray code of "_index"
	uint8_t _user_mask[32];   //!< User changes to the displayed matrix

	int bit_pos[8][8];  //!< Cells in this matrix are positions of bits
	
	BBitmap* _dotActive;		//!< Active dot in the matrix
	BBitmap* _dotInactive;		//!< Inactive dot in the matrix

	BDragger* _dragger;			//!< Replicant dragger

	BPath _settingsPath;
	BPoint _winPos;			//!< For saving and restoring window position
	bool   _isReplicant;	//!< A replicant does not restore window position
	
	static void LogToFile(const char* format, ...);	//!< For replicant debugging
	static void ClearLogFile();	//!< Delete the log file for replicant debugging
};

#endif
