/*
  ==============================================================================================================

    SpectralViewComponent.h
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include <algorithm>
#include <cmath>


//==============================================================================================================
/*
*/
class SpectralViewComponent    : public Component
{
public:
    static const unsigned lowerLimit; // Corresponds to 20 Hz, the lower end of human hearing.
    static const unsigned upperLimit; // Corresponds to 20560 Hz, the upper end of human hearing.
    
    SpectralViewComponent();
    ~SpectralViewComponent();

    void paint (Graphics&) override;
    void resized() override;

    
private:
    //==========================================================================================================
    //std::vector<Line<double>> grid;
    bool graphicsLocked;
    unsigned height; // Height of graphich component in pixels.
    unsigned width; // Height of graphich component in pixels.

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectralViewComponent)
};
