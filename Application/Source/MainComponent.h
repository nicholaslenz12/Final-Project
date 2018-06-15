/*
  ==============================================================================
 
    MainComponent.h
    Author:  Nicholas Lenz
 
   ==============================================================================
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

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

//==================================================================================
    
private:
    // Every text button will appear on the GUI.
    TextButton open;    // Object so that I can open a file.
    TextButton play;    // Object so that I can play the selected file.
    TextButton pause;   // Object so that I can pause the selected file.
    TextButton stop;    // Object so that I can stop the selected file.
    
    
    SpectralViewComponent spectrum;
    AudioTransportSource soundFile;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
    
};
