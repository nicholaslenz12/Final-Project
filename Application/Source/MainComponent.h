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
    /**
     Provides the functionality for the Open button being clicked. The functionality
        is based on the open button created as JUCE's tutorial "Making an Audio
        Player", as it provides the basics for opening a file for use in an
        application.
     */
    void openClicked();
    
    
    //==============================================================================================================
    
    // Every text button will appear on the GUI.
    TextButton open;    // Object so that I can open a file.
    TextButton play;    // Object so that I can play the selected file.
    TextButton pause;   // Object so that I can pause the selected file.
    TextButton stop;    // Object so that I can stop the selected file.
    
    
    SpectralViewComponent spectrum;
    AudioFormatManager filetypeManager;
    std::unique_ptr<AudioFormatReaderSource> fileSource;
    AudioTransportSource soundFile;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
    
};
