#include <Application.h>
#include <Window.h>
#include "MatrixView.h"

class GrayApp : public BApplication {
public:
    GrayApp() : BApplication("application/x-vnd.hitech.allPossibleImages") {}

    void ReadyToRun() override {
        BRect frame(100,
        			100,
        			100 + kCols * kDotSpacing,
        			100 + kRows * kDotSpacing);
        BWindow* win = new BWindow(frame, "API",
                                   B_TITLED_WINDOW, 
                                   B_QUIT_ON_WINDOW_CLOSE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE);
        win->AddChild(new MatrixView(win->Bounds(), "matrix"));
        win->Show();
    }
};

int main() {
    GrayApp app;
    app.Run();
    return 0;
}
