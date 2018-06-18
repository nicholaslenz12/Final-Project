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
class MeterComponent    :   public Component

{
public:
    //==========================================================================================================
    MeterComponent();
    ~MeterComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    
    //==========================================================================================================
    /**
     Using the buffer (dynamic array) from the main component and its corresponding size, computes the RMS -
         root mean sqaured value of the sampled waveform in the buffer. Since the waveform is discretized, the
         RMS is reduced to the square root of the average/mean of the squared samples in the buffer. The computed
         RMS is then used to set the Rectangle<float> peak's height to the RMS of the signal.
     */
    void createPeak(float* bufferToFill, int bufferSize);

    /**
     Computes the RMS of the samples in the buffer with bufferSize given.
     @param buffer     : The array of samples to compute the RMS from.
     @param bufferSize : The size of the previously mentioned array.
     */
    float computeRMS(float* buffer, int bufferSize);

private:
    //==========================================================================================================
    Label volumeBox;          // Textbox to change the gain (volume) of the signal.
    Rectangle<float> peak;    // Rectangle that represents the meter.
    
    bool graphicsLocked;      // If the graphics are locked (can't be drawn) is set to true.
    float RMS;                // The RMS of a signal.
    unsigned componentHeight; // Height of graphic component in pixels.
    unsigned componentWidth;  // Height of graphic component in pixels.
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterComponent)
};

#endif 
