/*
  ==============================================================================================================
 
    MainComponent.h
    Author:  Nicholas Lenz
 
   ==============================================================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectralViewComponent.h"

/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent
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
        Stopped,
        Paused,
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
     */
    void changeState(playState newState);
    
    //==============================================================================================================
    
    // Every text button will appear on the GUI.
    TextButton open;    // Object so that I can open a file.
    TextButton play;    // Object so that I can play the selected file.
    TextButton pause;   // Object so that I can pause the selected file.
    TextButton stop;    // Object so that I can stop the selected file.
    
    
    SpectralViewComponent spectrum; // Component that holds the functionality of the spectrum-viewer.
    AudioFormatManager filetypeManager; // Manages the file type (.mp3, .wav, ...).
    std::unique_ptr<AudioFormatReaderSource> fileSource; // Points to source properties of the file.
    AudioTransportSource projectSource; // Corresponds to the file loaded into the project and its timing
                                        // Within the project.
    playState state;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
    
};
