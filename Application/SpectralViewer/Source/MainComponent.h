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

/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
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
    
    /**
     Provides the functionality for the Open button being clicked. The functionality
        is based on the open button created as JUCE's tutorial "Build an audio
        player", as it provides the basics for opening a file for use in an
        application.
     */
    void openClicked();
    
    /**
     Provides the functionality for the Play button being clicked. Essentially, tells
        the currently focused spot in the project to move forward in the file.
     */
    void playClicked();
    
    /**
     Provides the functionality for the Puase button being clicked. Essentially, tells
        the currently focused spot in the project to remember its current position.
     */
    void pauseClicked();
    
    /**
     Provides the functionality for the Stop button being clicked. Essentially, tells
        the currently focused spot to move back to the start of the project.
     */
    void stopClicked();
    
    /**
     Implements how the app reacts to changes caused by pressing the play, pause, and
        stop buttons.
     @param newState : The new state of moving through the audio file.
     */
    void changeState(playState newState);
    
    /**
     Implements how the app will react when the stop button is pressed.
     @param button : The button that will be reacted to by a change in its state.
     */
    void buttonClicked(Button* button) override;

    /**
     This function allows the project to actually "listen" for button press and react to
        them. Also, since this function has been defined to the derived class MainComponent,
        MainComponent will not be abstract.
     @param source : The source of the change that will invoke a response.
     */
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    /**
     This function is called every x milliseconds specified by the function startTimer( x ).
     */
    void timerCallback() override;
    
    
    //==============================================================================================================
    
    // Every text button will appear on the GUI.
    TextButton open;    // Object so that I can open a file.
    TextButton play;    // Object so that I can play the selected file.
    TextButton pause;   // Object so that I can pause the selected file.
    TextButton stop;    // Object so that I can stop the selected file.
        
    SpectralViewComponent spectrum; // Component that holds the functionality of the spectrum-viewer.
    Slider transportProgress; // Object that shows how far through the audio file we are.
    
    
    AudioFormatManager filetypeManager; // Manages the file type (.mp3, .wav, ...).
    std::unique_ptr<AudioFormatReaderSource> fileSource; // Points to source properties of the file.
    AudioTransportSource projectSource; // Corresponds to the file loaded into the project and its timing
                                        // Within the project.
    playState state;                    // State of viewer: Playing, Starting, Stopping, Stopped, or Paused.
    
    double  projectTime;                // Current time at the source file.
    double  fileLength;                 // Length of the project in seconds.
    double  sampleFreq;                 // Sample rate of the selected file.
    double* buffer;         // Will be used to point to an array storing samples.
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
};

#endif
