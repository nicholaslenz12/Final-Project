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

SpectralViewComponent::SpectralViewComponent() : graphicsLocked(true)
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
    int order      = log2(maxBufferSize);
    int halfSize   = maxBufferSize/2;
    
    // Copies the elements from the audio buffer and puts it into an array of size 2^n, for some n.
    float* samplesForTransform = new float[maxBufferSize];
    std::copy(bufferToFill, bufferToFill + bufferSize, samplesForTransform);
    
    /* Then we apply the transform so that we map into the frequency vs. volume/amplitude domain. We need to
    construct the FFT object with half the size of smallest array of size 2^n that fits the buffer. We need to
    remember to normalize the values to be between -1 to 1. */
    dsp::FFT frequncyFFT(order - 1);
    frequncyFFT.performFrequencyOnlyForwardTransform(samplesForTransform);
    std::for_each(samplesForTransform, samplesForTransform + halfSize,
    [](float x) -> float
    {
        x = x/100;
        std::cout << x << " ";
        return x;

    });
    std::cout << "\n";
                  
//    for( auto i = 0; i < halfSize; ++i )
//    {
//        std::cout << samplesForTransform[i] << " ";
//    }
//    std::cout << "\n";
    
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Comment >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    
    double xCoord = 0;
    double step = 4*static_cast<double>(componentWidth)/halfSize;
    for( unsigned i = 0; i < halfSize/4; ++i )
    {
        int height = samplesForTransform[2*i] * componentHeight;

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
    
    // Draws "octave" lines, doesn't need to be locked because won't be redrawn on a buffer change.
//    for( int i = 0; i < 9; ++i )
//    {
//        unsigned x = (i+1)*componentWidth/10;
//        g.drawLine(x,0,x,componentWidth);
//    }
    if( !graphicsLocked )
    {
        for( auto rect : peaks )
        {
            g.drawRect(rect);
        }
    }
}

void SpectralViewComponent::resized()
{
    repaint();
    componentHeight = getHeight();
    componentWidth  = getWidth();
}
