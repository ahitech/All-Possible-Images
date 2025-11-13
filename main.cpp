#include <iostream>
#include <cstdint>
#include <signal.h>
#include <cstdlib>      // For std::exit()
#include <iomanip>

#include <Application.h>
#include <Window.h>
#include "MatrixView.h"

class GrayApp;
void signal_handler(int sig);

GrayApp* instance;
MatrixView*	fMatrixViewToBeSaved;

class MatrixWindow : public BWindow {
	public:
		using BWindow::BWindow;
		virtual void FrameMoved(BPoint origin) override {
			if (fMatrixViewToBeSaved) {
				fMatrixViewToBeSaved->SaveNewPosition(origin);
			}
		}
};


class GrayApp : public BApplication {
public:
    GrayApp() : BApplication("application/x-vnd.hitech.allPossibleImages") {}

    void ReadyToRun() override {
        BRect frame(100,
        			100,
        			100 + kCols * kDotSpacing + kDraggerSize,
        			100 + kRows * kDotSpacing + kDraggerSize);
        BWindow* win = new MatrixWindow(frame, "API",
                                   B_TITLED_WINDOW, 
                                   B_QUIT_ON_WINDOW_CLOSE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE);
		fMatrixViewToBeSaved = new MatrixView(win->Bounds(), "All Possible Images");
        win->AddChild(fMatrixViewToBeSaved);
        win->Show();
    }
    
    void SaveStateIfPossible() const {
    	if (fMatrixViewToBeSaved) {
    		std::cout << "In SaveStateIfPossible(): Calling to view's SaveState().\n";
    		fMatrixViewToBeSaved->SaveState();
    	}
    	else {
    		std::cout << "In SaveStateIfPossible(): Can't SaveState - no such view!\n";
    	}
    }
    
    virtual bool QuitRequested() override {
    	if (fMatrixViewToBeSaved) {
    		std::cout << "In QuitRequested(): Calling to view's SaveState().\n";
    		fMatrixViewToBeSaved->SaveState();
    	}
    	else {
    		std::cout << "In QuitRequested(): Can't SaveState - no such view!\n";
    	}
    	return BApplication::QuitRequested();
    }
};

int main() {
	signal(SIGINT, signal_handler);   // Ctrl+C
	signal(SIGTERM, signal_handler);  // kill <pid>
	signal(SIGHUP, signal_handler);   // Closing the TTY
	
    GrayApp app;
    instance = &app;
    app.Run();
    return 0;
}

void signal_handler(int sig) {
	std::cerr << "[!] Signal " << sig << " received.\n";

	GrayApp* app = instance;
	if (app) {
		std::cout << "Calling app->SaveStateIfPossible\n";
		app->SaveStateIfPossible();
	} else {
		std::cerr << "    ! MatrixView not available.\n";
	}

	std::exit(0);
}

