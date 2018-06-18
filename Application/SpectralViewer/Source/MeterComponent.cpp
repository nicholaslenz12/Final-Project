/*
  ==============================================================================

    MeterComponent.cpp
    Author:  Nicholas Lenz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MeterComponent.h"
#include <cmath>
#include <string>

//==============================================================================
MeterComponent::MeterComponent() :
                                    graphicsLocked(true),
                                    RMS(0.0),
                                    componentHeight(getHeight()),
                                    componentWidth(getWidth())

                                    

{
    addAndMakeVisible(&volumeBox);
    volumeBox.setColour(Label::backgroundColourId, Colour(255,212,22));
    volumeBox.setColour(Label::textColourId, Colours::black);
    volumeBox.setText("0.0 dBFS", dontSendNotification);
    volumeBox.setJustificationType(juce::Justification::centred);
}

MeterComponent::~MeterComponent() {}

void MeterComponent::createPeak(float* buffer, int bufferSize)
{
    graphicsLocked = true;
    float height = computeRMS(buffer, bufferSize) * componentHeight;
    peak.setSize(componentWidth, height);
    peak.setPosition(0, componentHeight*8/9 - height);
    graphicsLocked = false;
}

float MeterComponent::computeRMS(float* buffer, int bufferSize)
{
    float sum = 0;
    for( unsigned i = 0; i < bufferSize; ++i )
    {
        sum = sum + buffer[i]*buffer[i];
    }
    float newRMS = std::sqrt(sum/bufferSize);
    setRMS(newRMS);
    return newRMS;
}

void MeterComponent::setRMS(float newRMS)
{
    RMS = newRMS;
}

void MeterComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite);
    g.setColour(Colours::grey);
    
    for( int i = 1; i < 5; ++i )
    {
        unsigned lineHeight = 8*(componentHeight - componentHeight/(exp2(i)))/9;
        g.drawLine(0, lineHeight, componentWidth, lineHeight);
    }
    
    g.setColour(Colours::red);
    
    if( !graphicsLocked )
    {
        g.drawRect(peak);
        g.fillRect(peak);
    }
    
    if( RMS > 0.000001 )
    {
        volumeBox.setText(std::to_string(6*log2(RMS)) + " dBFS", dontSendNotification);
    }
    else
    {
        volumeBox.setText("-inf\ndBFS", dontSendNotification);
    }
    
}

void MeterComponent::resized()
{
    // Sets the region for the volume adjustment box.
    Rectangle<int> textArea(getWidth(),getHeight()/9);
    textArea.setPosition(0,getHeight()*8/9);
    volumeBox.setBounds(textArea);
    
    repaint();
    componentHeight = getHeight();
    componentWidth  = getWidth();
}
