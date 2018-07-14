/*
  ==============================================================================================================

    SpectralViewComponent.cpp
    Author:  Nicholas Lenz

  ==============================================================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectralViewComponent.h"

//==============================================================================================================
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
        graphicsLocked = true;
        
        

        graphicsLocked = false; // Allows the graphics to be redrawn.
    }
}

void SpectralViewComponent::paint (Graphics& g)
{
    g.fillAll(Colours::floralwhite); // Sets the color of the background to a nice color I found.
    
    g.setColour(Colours::lime); // Sets the colors of the peaks.
    
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
