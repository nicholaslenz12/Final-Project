/*
  ==============================================================================================================
 
    MainComponent.h
    Author:  Nicholas Lenz
 
   ==============================================================================================================
 */

#ifndef MainComponent_h
#define MainComponent_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectralViewComponent.h"
#include "MeterComponent.h"

class MainComponent   : public AudioAppComponent,
                        public ChangeListener,
                        public Button::Listener,
                        public Timer

{
public:
    MainComponent();
    ~MainComponent();

    //==============================================================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================================================
    void paint (Graphics& g) override;
    void resized() override;

//==================================================================================================================
    
private:
    //==============================================================================================================
    enum playState // Extending enumeration to the one found in JUCE's "Build an audio player".
    {
        Paused,
        Stopped,
        Starting,
        Playing,
        Stopping,
        Pausing
    };
    
    /** Provides the functionality for the Open button being clicked. The functionality
        is adapted from JUCE's tutorial "Build an audio player", as it provides the
        basics for opening a file for use in an application. My version sets the
        private variables for the MainComponent class during the file selection process.
     */
    void openClicked();
    
    /** Provides the functionality for the play button being clicked. Essentially, tells
        the currently focused spot in the project to move forward in the file.
     */
    void playClicked();
    
    /** Provides the functionality for the pause button being clicked. Essentially, tells
        the currently focused spot in the project to "remember" its current position.
     */
    void pauseClicked();
    
    /** Provides the functionality for the stop button being clicked. Essentially, tells
        the currently focused spot to move back to the start of the project.
     */
    void stopClicked();
    
    /** Implements how the app reacts to changes caused by pressing the play, pause, and
        stop buttons.
        @param newState : The new state of moving through the audio file.
     */
    void changeState(playState newState);
    
    /** Implements how the app will react when the stop button is pressed.
        @param button : The button that will be reacted to by a change in its state.
     */
    void buttonClicked(Button* button) override;

    /** This function allows the project to actually "listen" for button press and react to
        them. Also, since this function has been defined to the derived class MainComponent,
        MainComponent will not be abstract.
        @param source : The source of the change that will invoke a response.
     */
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    /** This function is called every 100 milliseconds specified by the function startTimer(100)
        called in the contructor of MainComponent.
     */
    void timerCallback() override;
    
    
    //==============================================================================================================
    
    // Text buttons that will appear on the GUI.
    TextButton open;
    TextButton play;
    TextButton pause;
    TextButton stop;
    
    // Label the percentage through the audio file.
    Label fiftyPer;
    Label twentyFivePer;
    Label seventyFivePer;
    
    // Components of the GUI.
    SpectralViewComponent spectrum;
    MeterComponent meter;
    Slider transportProgress;
    
    // Properties/managers of the source file.
    AudioFormatManager filetypeManager;
    std::unique_ptr<AudioFormatReaderSource> fileSource;
    AudioTransportSource projectSource;
    playState state;
    
    double fileLength;
    double sampleFreq;
    float* buffer;
    int    bufferSize;
    bool   wasPaused;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
};

#endif
