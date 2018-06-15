/*
  ==============================================================================================================

    SpectralViewComponent.cpp
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectralViewComponent.h"

//==============================================================================================================
SpectralViewComponent::SpectralViewComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    setSize(400, 300);
    
}

SpectralViewComponent::~SpectralViewComponent()
{
}

void SpectralViewComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    g.fillAll(Colour(255,154,22)); // Sets the color of the background to a nice color I found.
    
//    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
//
//    g.setColour (Colours::white);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
//
//    g.setColour (Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("SpectralViewComponent", getLocalBounds(),
//                Justification::centred, true);   // draw some placeholder text
}

void SpectralViewComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
