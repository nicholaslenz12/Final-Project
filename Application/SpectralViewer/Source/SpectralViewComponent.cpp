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

SpectralViewComponent::SpectralViewComponent() :    graphicsLocked(true),
                                                    orderFFT(9)

{
    setSize(400, 300);
    componentHeight = getHeight();
    componentWidth  = getWidth();
}

SpectralViewComponent::~SpectralViewComponent() {}

void SpectralViewComponent::createPeaks(float* bufferToFill, int bufferSize)
{
    graphicsLocked = true;
    peaks.clear();
    /* The goal is to fit our buffer into a array to which we can apply an FFT transform. In order to do so,
    we must create a array with 2^n elements, if the buffer is too small, we just put the samples we have in. */
    int maxBufferSize  = juce::nextPowerOfTwo(bufferSize);
    int orderFFT       = log2(maxBufferSize);
    int halfSize       = maxBufferSize/2;
    
    // Copies the elements from the audio buffer and puts it into an array of size 2^n, for some n.
    float* samplesForTransform = new float[maxBufferSize];
    std::copy(bufferToFill, bufferToFill + bufferSize, samplesForTransform);
    
    /* Then we apply the transform so that we map into the frequency vs. volume/amplitude domain. We construct
    the FFT object with half the size of smallest array of size 2^n that fits the buffer. We need to remember
    to normalize the values to be between -1 to 1. You can also note that we only care about the first half of
    the array, because the FFT returns a "symmetric" array, i.e. one where the ith index is equal to the
    (size - i)th index. */
    dsp::FFT frequncyFFT(orderFFT - 1);
    frequncyFFT.performFrequencyOnlyForwardTransform(samplesForTransform);
//    std::for_each(samplesForTransform, samplesForTransform + halfSize,
//    [](float x) -> float
//    {
//        return x/100;
//    });
    for( auto i = 0; i < halfSize; ++i )
    {
        samplesForTransform[i] = samplesForTransform[i]/100;
    }
    
    double xCoord   = 0;
    int    logscale = 1;
    double step = static_cast<double>(componentWidth)/(orderFFT-1);
    
    for( unsigned i = 0; i < orderFFT - 1; ++i )
    {
        if( i > 0 )
        {
            logscale *= 2;
        }
        int height = samplesForTransform[logscale] * componentHeight;

        Rectangle<float> r;
        if( step >= 0 && height >= 0 )
        {
            r.setSize(step, height);
            r.setPosition(xCoord, componentHeight - height);
            peaks.push_back(r);
        }
        
        xCoord += step;
    }
    
    graphicsLocked = false;
    
}

void SpectralViewComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite); // Sets the color of the background to a nice color I found.
    
    g.setColour(Colours::grey);
    
    //Draws "octave" lines, doesn't need to be locked because won't be redrawn on a buffer change.
    for( int i = 0; i < orderFFT - 1 ; ++i )
    {
        unsigned x = (i+1)*componentWidth/(orderFFT - 1);
        g.drawLine(x,0,x,componentWidth);
    }
    
    //Draws volume lines, each line corresponds to 6 decibels lower than the line above it.
    for( int i = 1; i < 6; ++i )
    {
        unsigned lineHeight = componentHeight - componentHeight/(exp2(i));
        g.drawLine(0, lineHeight, componentWidth, lineHeight);
    }
    
    
    if( !graphicsLocked )
    {
        for( auto rect : peaks )
        {
            g.drawRect(rect);
            g.fillRect(rect);
        }
    }
}

void SpectralViewComponent::resized()
{
    repaint();
    componentHeight = getHeight();
    componentWidth  = getWidth();
}
