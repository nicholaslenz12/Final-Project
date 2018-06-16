/*
  ==============================================================================================================

    SpectralViewComponent.h
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================================================
/*
*/
class SpectralViewComponent    : public Component
{
public:
    SpectralViewComponent();
    ~SpectralViewComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectralViewComponent)
};
