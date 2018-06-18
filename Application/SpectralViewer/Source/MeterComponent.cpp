/*
  ==============================================================================

    MeterComponent.cpp
    Author:  Nicholas Lenz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MeterComponent.h"
#include <cmath>

//==============================================================================
MeterComponent::MeterComponent() :
                                    graphicsLocked(true),
                                    componentWidth(getWidth()),
                                    componentHeight(getHeight())

{
    
}

MeterComponent::~MeterComponent() {}

void MeterComponent::createPeak(float* buffer, int bufferSize)
{
    graphicsLocked = true;
    float height = computeRMS(buffer, bufferSize) * componentHeight;
    peak.setSize(componentWidth, height);
    peak.setPosition(0, componentHeight - height);
    graphicsLocked = false;
}

float MeterComponent::computeRMS(float* buffer, int bufferSize)
{
    float sum = 0;
    for( unsigned i = 0; i < bufferSize; ++i )
    {
        sum = sum + buffer[i]*buffer[i];
    }
    return std::sqrt(sum/bufferSize);
}

void MeterComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite);
    g.setColour(Colours::grey);
    
    for( int i = 1; i < 5; ++i )
    {
        unsigned lineHeight = componentHeight - componentHeight/(exp2(i));
        g.drawLine(0, lineHeight, componentWidth, lineHeight);
    }
    
    g.setColour(Colours::red);
    
    if( !graphicsLocked )
    {
        g.drawRect(peak);
        g.fillRect(peak);
    }
}

void MeterComponent::resized()
{
    repaint();
    componentHeight = getHeight();
    componentWidth  = getWidth();
}
