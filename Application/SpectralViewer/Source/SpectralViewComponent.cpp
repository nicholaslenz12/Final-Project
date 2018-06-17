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
    peaks.resize(10);
}

SpectralViewComponent::~SpectralViewComponent() {}

void SpectralViewComponent::createPeaks(double* bufferToFill, int bufferSize)
{
    graphicsLocked = true;
    /* The goal is to fit our buffer into a bin to which we can apply an FFT transform. In order to do so,
    we must create a bin with 2^n elements, if the buffer is too small, we just put the samples we have in. */
    int fftSize  = juce::nextPowerOfTwo(bufferSize);
    int order    = log2(fftSize);
    int halfSize = fftSize/2;
    
}

void SpectralViewComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite); // Sets the color of the background to a nice color I found.
    
    // Draws octave lines, doesn't need to be locked because won't be redrawn on a buffer change
    for( int i = 0; i < 9; ++i )
    {
        unsigned x = (i+1)*width/10;
        g.drawLine(x,0,x,height);
    }
    if( !graphicsLocked )
    {
        
    }
}

void SpectralViewComponent::resized()
{
    repaint();
    height = getHeight();
    width  = getWidth();
}
