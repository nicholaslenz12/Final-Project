/*
  ==============================================================================================================

    MeterComponent.h
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#ifndef MeterComponent_h
#define MeterComponent_h

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================================================
/*
*/
class MeterComponent    :   public Component,
                            public Timer
{
public:
    MeterComponent();
    ~MeterComponent();
    
    void createPeak(float* bufferToFill, int bufferSize);

    float computeRMS(float* buffer, int bufferSize);
    
    void setRMS(float oldRMS);

    void timerCallback() override;
    
    void paint (Graphics&) override;
    void resized() override;

private:
    //==========================================================================================================
    Label volumeBox;          // Textbox to change the gain (volume) of the signal.
    Rectangle<float> peak;
    
    bool graphicsLocked;
    float RMS;
    unsigned componentHeight; // Height of graphic component in pixels.
    unsigned componentWidth; // Height of graphic component in pixels.
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterComponent)
};

#endif 
