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
/*
*/
class SpectralViewComponent     : public Component

{
public:
    static const unsigned lowerLimit; // Corresponds to 20 Hz, the lower end of human hearing.
    static const unsigned upperLimit; // Corresponds to 20560 Hz, just above the upper end of human hearing.
    
    SpectralViewComponent();
    ~SpectralViewComponent();
    
    void createPeaks(float* bufferToFill, int bufferSize);

    void paint (Graphics&) override;
    void resized() override;

    
private:
    //==========================================================================================================
    std::vector<Rectangle<float>> peaks;
    bool graphicsLocked;
    unsigned componentHeight; // Height of graphic component in pixels.
    unsigned componentWidth; // Height of graphic component in pixels.


    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectralViewComponent)
};

#endif
