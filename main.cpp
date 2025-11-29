#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <Message.h>
#include <Font.h>
#include <StopWatch.h>
#include <MessageRunner.h>

#include <stdio.h>

// Message constants
const uint32 START_STOP_MSG = 'StSp';
const uint32 RESET_MSG = 'Rset';
const uint32 UPDATE_MSG = 'Updt';

class StopwatchWindow : public BWindow {
public:
    StopwatchWindow() 
        : BWindow(BRect(100, 100, 530, 400), "Haiku OS: Stopwatch", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
    {
        // Create main view with Haiku-like background
        BView *mainView = new BView(Bounds(), "mainView", B_FOLLOW_ALL, B_WILL_DRAW);
        mainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
        AddChild(mainView);
        
        // Create title
        BStringView *titleLabel = new BStringView(BRect(20, 20, 410, 80), "title", "Stopwatch:");
        titleLabel->SetFont(be_bold_font);
        titleLabel->SetFontSize(60);
        mainView->AddChild(titleLabel);
        
        // Create time display
        timeLabel = new BStringView(BRect(20, 90, 410, 170), "timeLabel", "00:00:00");
        timeLabel->SetFont(be_bold_font);
        timeLabel->SetFontSize(32);
        timeLabel->SetAlignment(B_ALIGN_CENTER);
        mainView->AddChild(timeLabel);
        
        // Create start/pause button
        startButton = new BButton(BRect(20, 200, 200, 250), "startButton", "Start", 
                                 new BMessage(START_STOP_MSG));
        mainView->AddChild(startButton);
        
        // Create reset button
        BButton *resetButton = new BButton(BRect(220, 200, 400, 250), "resetButton", "Reset",
                                          new BMessage(RESET_MSG));
        mainView->AddChild(resetButton);
        
        stopwatch = NULL;
        pausedTime =  0;
        isRunning = false;
        isPaused = false;
        
        // Start update timer
        updateTimer = new BMessageRunner(BMessenger(this), new BMessage(UPDATE_MSG),200000);
    }
    
    ~StopwatchWindow() {
        delete stopwatch;
        delete updateTimer;
    }
    
    void MessageReceived(BMessage *message) {
        switch(message->what) {
            case START_STOP_MSG:
                HandleStartStop();
                break;
                
            case RESET_MSG:
                HandleReset();
                break;
                
            case UPDATE_MSG:
                UpdateDisplay();
                break;
                
            default:
                BWindow::MessageReceived(message);
        }
    }
    
    bool QuitRequested() {
        be_app->PostMessage(B_QUIT_REQUESTED);
        return true;
    }
    
private:
    void HandleStartStop() {
        if (!isRunning && !isPaused) {
            // Start
            stopwatch = new BStopWatch("stopwatch", true);
            isRunning = true;
            startButton->SetLabel("Pause");
        } else if (isRunning) {
            // Pause
            pausedTime += stopwatch->ElapsedTime() / 1000000.0; // Convert to seconds
            delete stopwatch;
            stopwatch = NULL;
            isRunning = false;
            isPaused = true;
            startButton->SetLabel("Continue");
        } else if (isPaused) {
            // Continue
            stopwatch = new BStopWatch("stopwatch", true);
            isRunning = true;
            isPaused = false;
            startButton->SetLabel("Pause");
        }
    }
    
    void HandleReset() {
        delete stopwatch;
        stopwatch = NULL;
        isRunning = false;
        isPaused = false;
        pausedTime = 0;
        startButton->SetLabel("Start");
        timeLabel->SetText("00:00:00");
    }
    
    void UpdateDisplay() {
        if (isRunning && stopwatch) {
            double elapsed = (stopwatch->ElapsedTime() / 1000000.0) + pausedTime;
            
            int hours = (int)(elapsed / 3600);
            int minutes = (int)((elapsed - (hours * 3600)) / 60);
            int seconds = (int)(elapsed - (hours * 3600) - (minutes * 60));
            
            char timeStr[256];
            snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hours, minutes, seconds);
            timeLabel->SetText(timeStr);
        }
    }
    
    BButton *startButton;
    BStringView *timeLabel;
    BStopWatch *stopwatch;
    BMessageRunner *updateTimer;
    double pausedTime;
    bool isRunning;
    bool isPaused;
};

class StopwatchApp : public BApplication {
public:
    StopwatchApp() : BApplication("application/x-vnd.Stopwatch-Demo") {
    }
    
    void ReadyToRun() {
        StopwatchWindow *window = new StopwatchWindow();
        window->Show();
    }
};

int main() {
    StopwatchApp app;
    app.Run();
    return 0;
}
