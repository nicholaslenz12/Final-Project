/*
  ==============================================================================================================

    SpectralViewComponent.cpp
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectralViewComponent.h"

//==============================================================================================================
const unsigned SpectralViewComponent::lowerLimit = 20;
const unsigned SpectralViewComponent::upperLimit = 20560;

SpectralViewComponent::SpectralViewComponent() : graphicsLocked(false)
{
    setSize(400, 300);
    height = getHeight();
    width  = getWidth();
    //grid.resize(10);
}

SpectralViewComponent::~SpectralViewComponent()
{
}

void SpectralViewComponent::paint (Graphics& g)
{
    g.fillAll(Colour(255,154,22)); // Sets the color of the background to a nice color I found.
    if( !graphicsLocked )
    {
        for( int i = 0; i < 9; ++i )
        {
            unsigned x = (i+1)*width/10;
            g.drawLine(x,0,x,height);
        }
    }
    //g.drawLine(50,0,50,getBottom());
}

void SpectralViewComponent::resized()
{
    repaint();
    height = getHeight();
    width  = getWidth();
}
