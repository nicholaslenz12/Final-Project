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
                                                    componentHeight(getHeight()),
                                                    componentWidth(getWidth()),
                                                    orderFFT(9)

{
    // Adds the scale (in dBFS) to GUI.
    addAndMakeVisible(&minusSixDecibels);
    addAndMakeVisible(&minusTwelveDecibels);
    addAndMakeVisible(&minusEighteenDecibels);
    addAndMakeVisible(&minusTwentyFourDecibels);
    
    // Sets the text for the scale, and positions them.
    minusSixDecibels.setText("-6 dBFS", dontSendNotification);
    minusSixDecibels.setColour(Label::textColourId, Colours::black);
    minusSixDecibels.setJustificationType(Justification::right);
    minusTwelveDecibels.setText("-12 dBFS", dontSendNotification);
    minusTwelveDecibels.setColour(Label::textColourId, Colours::black);
    minusTwelveDecibels.setJustificationType(Justification::right);
    minusEighteenDecibels.setText("-18 dBFS", dontSendNotification);
    minusEighteenDecibels.setColour(Label::textColourId, Colours::black);
    minusEighteenDecibels.setJustificationType(Justification::right);
    minusTwentyFourDecibels.setText("-24 dBFS", dontSendNotification);
    minusTwentyFourDecibels.setColour(Label::textColourId, Colours::black);
    minusTwentyFourDecibels.setJustificationType(Justification::right);
        
}

SpectralViewComponent::~SpectralViewComponent() {}

void SpectralViewComponent::createPeaks(float* bufferToFill, int bufferSize)
{
    if( !isMouseButtonDown() )
    {
        graphicsLocked = true; // Locks the graphics.
        peaks.clear(); // Clears peaks to be refilled.
    
        /*
            The goal is to fit our buffer into a array to which we can apply an FFT transform. In order to do so,
            we must create a array with 2^n elements, if the buffer is too small, we just put the samples we
            have in. the reason for 2^n elements is the standard FFT uses a divided and conquer algorithm, which
            splits the buffer in half successively.
        */
        int maxBufferSize  = juce::nextPowerOfTwo(bufferSize);
        int orderFFT       = log2(maxBufferSize);
        int halfSize       = maxBufferSize/2;
    
        // Copies the elements from the audio buffer and puts it into an array of size 2^n, for some n.
        float* samplesForTransform = new float[maxBufferSize];
        std::copy(bufferToFill, bufferToFill + bufferSize, samplesForTransform);
    
        /*
            Then we apply the transform so that we map into the frequency vs. volume/amplitude domain. We
            construct the FFT object with half the size of smallest array of size 2^n that fits the buffer. We
            need to remember to normalize the values to be between -1 to 1. You can also note that we only care
            about the first half of the array, because the FFT returns a "symmetric" array, i.e. one where the
            ith index is equal to the (size - i)th index.
        */
        dsp::FFT frequncyFFT(orderFFT - 1);
        frequncyFFT.performFrequencyOnlyForwardTransform(samplesForTransform);
        
        // Normalizes all the frequencies so their maximum volume is 1.
        std::transform(samplesForTransform, samplesForTransform + halfSize, samplesForTransform,
                       [](float& x){ return x/100; });
        
        double xCoord   = 0; // x coordinate for the left-most point of each octave, will be incremented.
        int    logscale = 1; // Used to scale the frequency spectrum, effectively squashing upper frequencies,
                             // see ResearchDSP.txt for a longer discussion.
        double step = static_cast<double>(componentWidth)/(orderFFT-1); // Width of each octave.
    
        for( unsigned i = 0; i < (orderFFT-1); ++i ) // For each octave
        {
            if( i > 0 )
            {
                logscale *= 2; // Makes it so the current octave has twice the # of frequencies as the previous.
            }
        
            // Initilize variables.
            int halfLogScale = logscale/2;
            float sum = 0;
        
            for( unsigned j = halfLogScale; j < logscale; ++j) // For all the frequencies from the FFT in the octave.
            {
                sum = sum + samplesForTransform[j + 1]; // Sum them.
            }
        
            float average;
            if( i > 0 )
            {
                average = sum/halfLogScale; // Then average them together.
            }
            else
            {
                average = sum;
            }
        
            if( average > 1 ) // If the average is greater than 1, set to 1 so can fit in the spectrum.
            {
                average = 1;
            }

            // Sets the height of each band (octave), scaled so that higher frequencies appear louder.
            int height =  (4.0/6)*logscale * average * componentHeight;
        
            if( height > componentHeight )  // Resets if height is too large (doesn't happen that frequently though).
            {
                height = componentHeight;
            }
        

            // Creates and fills the appropriate rectangle with height as computed previously.
            Rectangle<float> rect;
            if( step >= 0 && height >= 0 )
            {
                rect.setSize(step, height);
                rect.setPosition(xCoord, componentHeight - height);
                peaks.push_back(rect);
            }
            xCoord += step; // Increments so the next ocave is drawn to the right of the previous.
        }
        
        // Frees up dynamic memory.
        delete[] samplesForTransform;
        samplesForTransform = nullptr;

        graphicsLocked = false; // Allows the graphics to be redrawn.
    }
}

void SpectralViewComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite); // Sets the color of the background to a nice color I found.
    
    g.setColour(Colours::lime); // Sets the colors of the peaks.
    if( !graphicsLocked ) // If the graphics are not locked (not computing FFT and creating peaks).
    {
        for( auto rect : peaks ) // Loops through the container, drawing and coloring each rectangle.
        {
            g.drawRect(rect);
            g.fillRect(rect);
        }
    }
    
    g.setColour(Colours::grey); // Sets the colors of the lines.
    //Draws "octave" lines, doesn't need to be locked because won't be redrawn on a buffer change.
    for( int i = 0; i < orderFFT - 1 ; ++i )
    {
        unsigned x = (i+1)*componentWidth/(orderFFT - 1);
        g.drawLine(x,0,x,componentWidth);
    }
    //Draws volume lines, each line corresponds to 6 decibels lower than the line above it.
    for( int i = 1; i < 5; ++i )
    {
        unsigned lineHeight = componentHeight - componentHeight/(exp2(i));
        g.drawLine(0, lineHeight, componentWidth, lineHeight);
    }
}

void SpectralViewComponent::resized()
{
    // Sets the region for the -6 dBFS line marker.
    Rectangle<int> sixArea(getWidth()/10, getHeight()/30);
    sixArea.setPosition(getWidth()*18/20, getHeight()/2);
    minusSixDecibels.setBounds(sixArea);
    
    // Sets the region for the -12 dBFS line marker.
    Rectangle<int> twelveArea(getWidth()/10, getHeight()/30);
    twelveArea.setPosition(getWidth()*18/20, getHeight()*3/4);
    minusTwelveDecibels.setBounds(twelveArea);
    
    // Sets the region for the -18 dBFS line marker.
    Rectangle<int> eighteenArea(getWidth()/10, getHeight()/30);
    eighteenArea.setPosition(getWidth()*18/20, getHeight()*7/8);
    minusEighteenDecibels.setBounds(eighteenArea);
    
    // Sets the region for the -24 dBFS line marker.
    Rectangle<int> twentyFourArea(getWidth()/10, getHeight()/30);
    twentyFourArea.setPosition(getWidth()*18/20, getHeight()*15/16);
    minusTwentyFourDecibels.setBounds(twentyFourArea);
    
    repaint();
    componentHeight = getHeight(); 
    componentWidth  = getWidth();
}
