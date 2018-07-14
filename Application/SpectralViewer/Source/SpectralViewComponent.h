/*
  ==============================================================================================================

    SpectralViewComponent.h
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#ifndef SpectralViewComponent_h
#define SpectralViewComponent_h

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include <algorithm>
#include <cmath>


//==============================================================================================================
class SpectralViewComponent     :   public Component

{
public:
    
    //==========================================================================================================
    SpectralViewComponent();
    ~SpectralViewComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    
    //==========================================================================================================
    /**
     Takes a buffer filled with samples, applies JUCE's built in FFT routine to the buffer, which returns a
         symmetric array of values, where each value represents a frequency, spaced from 0 Hz to the Nyquist
         frequency. Each element of the array is then scaled by the size of the local component (the spectrum
         display). Then each element is used to create a rectangle of height corresponding to the ampltidue of
         the signal, and width corresponding to the spacing of the FFT.
     @param bufferToFill : The array of samples that are converted to the frequency domain.
     @param bufferSize   : The size of the array of samples.
     */
    void createPeaks(float* bufferToFill, int bufferSize);

//==============================================================================================================
    
private:
    //==========================================================================================================
    Label minusSixDecibels;
    Label minusTwelveDecibels;
    Label minusEighteenDecibels;
    Label minusTwentyFourDecibels;
    
    std::vector<Rectangle<float>> peaks; // Stores the rectangles that form the spectrum display.
    bool graphicsLocked; // Boolean that returns false if the FFT routine is running.
    unsigned componentHeight; // Height of graphic component in pixels.
    unsigned componentWidth;  // Width of graphic component in pixels.
    int      orderFFT;        // Order of the FFT.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectralViewComponent)
};

#endif
