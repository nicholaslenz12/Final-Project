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
}

SpectralViewComponent::~SpectralViewComponent() {}

void SpectralViewComponent::createPeaks(float* bufferToFill, int bufferSize)
{
    graphicsLocked = true;
    peaks.clear();
    /* The goal is to fit our buffer into a array to which we can apply an FFT transform. In order to do so,
    we must create a array with 2^n elements, if the buffer is too small, we just put the samples we have in. */
    int maxBufferSize  = juce::nextPowerOfTwo(bufferSize);
    int order      = log2(maxBufferSize);
    int halfSize   = maxBufferSize/2;
    
    // Copies the elements from the audio buffer and puts it into an array of size 2^n, for some n.
    float* samplesForTransform = new float[maxBufferSize];
    std::copy(bufferToFill, bufferToFill + bufferSize, samplesForTransform);
    
    /* Then we apply the transform so that we map into the frequency vs. volume/amplitude domain. We need to
    construct the FFT object with half the size of smallest array of size 2^n that fits the buffer We need to
    remember to normalize the values to be between -1 to 1. */
    dsp::FFT frequncyFFT(order - 1);
    frequncyFFT.performFrequencyOnlyForwardTransform(samplesForTransform);
    std::for_each(samplesForTransform, samplesForTransform + halfSize, [maxBufferSize](float x)
                                                                       { log(x / maxBufferSize); });
    for( unsigned i = 0; i < 5; ++i )
    {
        
    }
    
    graphicsLocked = false;
    
}

void SpectralViewComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite); // Sets the color of the background to a nice color I found.
    
    // Draws "octave" lines, doesn't need to be locked because won't be redrawn on a buffer change.
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
