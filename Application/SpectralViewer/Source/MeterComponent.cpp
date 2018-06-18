/*
  ==============================================================================================================

    MeterComponent.cpp
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MeterComponent.h"
#include <cmath>
#include <string>

//==============================================================================================================
MeterComponent::MeterComponent() :  graphicsLocked(true),
                                    RMS(0.0),
                                    componentHeight(getHeight()),
                                    componentWidth(getWidth())

{
    // Adds the meter's numberical display to the GUI. Sets the value to zero at first.
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
    
    float height = computeRMS(buffer, bufferSize) * componentHeight; // Computes the height of the meter.
    peak.setSize(componentWidth, height);              // Sets the size of the meter.
    peak.setPosition(0, componentHeight*8/9 - height); // Sets the position ofhte meter in its parent component.
    
    graphicsLocked = false;
}

float MeterComponent::computeRMS(float* buffer, int bufferSize)
{
    float sum = 0;
    for( unsigned i = 0; i < bufferSize; ++i ) // Computes the sum of the squares of the samples.
    {
        sum = sum + buffer[i]*buffer[i];
    }
    float newRMS = std::sqrt(sum/bufferSize);  // Computes the RMS.
    RMS = newRMS;  // Sets the private variable named RMS to the new RMS.
    return newRMS; // Returns the new RMS.
}

void MeterComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite); // Colors the backgounds.
    g.setColour(Colours::grey);      // Sets the color for the volume lines.
    
    for( int i = 1; i < 5; ++i )     // Draws the volume lines.
    {
        unsigned lineHeight = 8*(componentHeight - componentHeight/(exp2(i)))/9;
        g.drawLine(0, lineHeight, componentWidth, lineHeight);
    }
    
    g.setColour(Colours::red); // Sets the color of the meter.
    
    if( !graphicsLocked ) // If the graphics are not locked draws the meter.
    {
        g.drawRect(peak);
        g.fillRect(peak);
    }
    
    if( RMS > 0.000001 )  // If the RMS is not too small, computes the computes the dBFS from the normalized value.
    {
        volumeBox.setText(std::to_string(6*log2(RMS)) + " dBFS", dontSendNotification);
    }
    else // Otherwise, sets the volume to -inf.
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
