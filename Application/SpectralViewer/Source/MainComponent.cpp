/*
  ==============================================================================================================
 
    MainComponent.cpp
    Author:  Nicholas Lenz
 
  ==============================================================================================================
*/

#include "MainComponent.h"
#include "SpectralViewComponent.h"

//==============================================================================================================
MainComponent::MainComponent() :    state(Stopped),
                                    projectTime(0.0),
                                    fileLength(1),
                                    sampleFreq(44100)

{
    // Sets the size of the application.
    setSize (800, 600);

    // Specifies the number of input and output channels that we want to open.
    setAudioChannels (2, 2);
    
    // Makes all the buttons visibible.
    addAndMakeVisible(&open);
    addAndMakeVisible(&play);
    addAndMakeVisible(&pause);
    addAndMakeVisible(&stop);
    
    // Sets what all the text buttons say.
    open.setButtonText("---> Open a File <---");
//    play.setButtonText("Play");
//    pause.setButtonText("Pause");
//    stop.setButtonText("Stop");
    
    // Disable all the buttons at the start.
    play.setEnabled(false);
    pause.setEnabled(false);
    stop.setEnabled(false);
    
    // Sets the colors of the buttons to make them look nicer.
    open.setColour(TextButton::buttonColourId, Colour(255,212,22));
    open.setColour(TextButton::textColourOffId, Colours::black);
    play.setColour(TextButton::buttonColourId, Colours::green);
    play.setColour(TextButton::textColourOffId, Colours::black);
    pause.setColour(TextButton::buttonColourId, Colour(255,212,22));
    pause.setColour(TextButton::textColourOffId, Colours::black);
    stop.setColour(TextButton::buttonColourId, Colours::red);
    stop.setColour(TextButton::textColourOffId, Colours::black);
    
    // Provides the functionality for the buttons, i.e. once they are clicked
    open.onClick  = [this] { openClicked(); };
    play.onClick  = [this] { playClicked(); };
    pause.onClick = [this] { pauseClicked(); };
    stop.onClick  = [this] { stopClicked(); };
    stop.addListener(this);
    
    // Makes the spectrum analyzer, meter, and sliders visible.
    addAndMakeVisible(&spectrum);
    addAndMakeVisible(&meter);
    addAndMakeVisible(&transportProgress);
    addAndMakeVisible(&volumeBox);
    
    // Sets the color of the volume box.
    volumeBox.setColour(Label::backgroundColourId, Colour(255,212,22));
    volumeBox.setColour(Label::textColourId, Colours::black);
    volumeBox.setText("0.0", dontSendNotification);
    volumeBox.setJustificationType(juce::Justification::centred);
    
    
    // Customizes the display of the audio progress bar to display the percent through the audio file.
    transportProgress.setRange(0, 100);
    transportProgress.setTextValueSuffix(" %");
    transportProgress.setNumDecimalPlacesToDisplay(1);
    transportProgress.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 60);
    transportProgress.setColour(juce::Slider::backgroundColourId, Colours::black);
    transportProgress.setColour(juce::Slider::trackColourId, Colour(255,212,22));
    transportProgress.setColour(juce::Slider::thumbColourId, Colour(255,212,22));
    transportProgress.setColour(juce::Slider::textBoxBackgroundColourId, Colour(255,212,22));
    transportProgress.setColour(juce::Slider::textBoxTextColourId, Colours::black);
    
    // Customizes the style and range of the rotary dial.
    
    startTimer(100); // Starts the internal timing for the project. Corresponds to 1000/100 = 10 frames/second.
    filetypeManager.registerBasicFormats(); // Allows the user to select standard audio filetypes.
    projectSource.addChangeListener(this); // Adds a listener for button presses.
    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================================================
void MainComponent::openClicked()
{
    FileChooser findFile("Select an audio file to play.", File::nonexistent, "");
    
    if( findFile.browseForFileToOpen() ) // If the native file browser opens and the user opens a file.
                                          
    {
        auto selectedFile = findFile.getResult(); // Sets the selected file to the one choosen.
        auto* fileReader = filetypeManager.createReaderFor(selectedFile); // Creates a file reader.
        if( fileReader ) // If successful.
        {
            // Access the necessary information from the file such as its sample rate. Also, we choose if
            // the file will be segmented and processed on separate threads (which we are not going to do).
            std::unique_ptr<AudioFormatReaderSource> selectedFileSource
                                                     (new AudioFormatReaderSource(fileReader,true));
            projectSource.setSource(selectedFileSource.get(), 0, nullptr, fileReader->sampleRate);
            sampleFreq = fileReader->sampleRate;
            fileLength = (static_cast<double>(fileReader->lengthInSamples)/sampleFreq);
            play.setEnabled(true); // Prepares the play button so that it can play.
            fileSource.reset(selectedFileSource.release());
        }
        open.setButtonText(selectedFile.getFileName());
        
    }
}

void MainComponent::playClicked()
{
    pause.setToggleState(false, NotificationType::dontSendNotification);
    stop.setToggleState(false, NotificationType::dontSendNotification);
    changeState(Starting);
}

void MainComponent::pauseClicked()
{

    pause.setToggleState(true, NotificationType::dontSendNotification);
    stop.setToggleState(false, NotificationType::dontSendNotification);
    changeState(Pausing);
}

void MainComponent::stopClicked()
{
    pause.setToggleState(false, NotificationType::dontSendNotification);
    stop.setToggleState(true, NotificationType::dontSendNotification);
    changeState(Stopping);
}

void MainComponent::changeState(playState newState)
{
    if(state != newState)
    {
        state = newState;
        
        switch(state)
        {
            case Paused:
                stop.setEnabled(true);
                pause.setEnabled(false);
                play.setEnabled(true);
                break;
            case Stopped:
                stop.setEnabled(false);
                pause.setEnabled(false);
                play.setEnabled(true);
                projectSource.setPosition(0.0);
                break;
            case Starting:
                play.setEnabled(false);
                projectSource.start();
                break;
            case Playing:
                stop.setEnabled(true);
                pause.setEnabled(true);
                break;
            case Pausing:
                projectSource.stop();
                break;
            case Stopping:
                projectSource.stop();
                break;
        }
    }
}

void MainComponent::buttonClicked(Button* button)
{
    if( button == &stop )
    {
        changeState(Stopped);
    }
}

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if( source == &projectSource ) // If the source of the callback is the project's source audio file.
    {
        if( projectSource.isPlaying() ) // Checks to see if the project is playing.
        {
            changeState(Playing); // If so changes the state to playing.
        }
        else if( pause.getToggleState() )
        {
            changeState(Paused);
        }
        else
        {
            changeState(Stopped);
        }
    }
}

void MainComponent::timerCallback()
{
    projectTime = projectSource.getCurrentPosition();
    transportProgress.setValue(projectTime);
    double percentThrough = 100*projectTime/fileLength;
    transportProgress.setValue(percentThrough);
}


//==============================================================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    projectSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    buffer = new float[samplesPerBlockExpected];
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& filledBuffer)
{
    if( !(fileSource.get()) ) // If there is no current file loaded.
    {
        filledBuffer.clearActiveBufferRegion(); // Clear everything so there's no noise.
    }
    else
    {
        projectSource.getNextAudioBlock(filledBuffer); // Fill the next audio block with the appropriate samples.
    }
    
    // Gets a pointer to the first sample in the first audio channel.
    auto* bufferPtr = filledBuffer.buffer->getReadPointer(0, filledBuffer.startSample);
    int bufferSize = filledBuffer.numSamples;

    for( auto i = 0; i < bufferSize; ++i )
    {
        buffer[i] = bufferPtr[i];
    }
    meter.createPeak(buffer, filledBuffer.numSamples);
    spectrum.createPeaks(buffer, filledBuffer.numSamples);

    

}

void MainComponent::releaseResources()
{
    projectSource.releaseResources();
}

//==============================================================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black); // Makes the background black.
    spectrum.repaint();
}

void MainComponent::resized()
{
    Rectangle<int> applicationArea = getLocalBounds(); // Total area of the GUI.
    
    // Sets the region for the open file button.
    Rectangle<int> openArea(getWidth(), getHeight()/10);
    openArea.setTop(getLocalBounds().getY());
    openArea.setLeft(getLocalBounds().getX());
    open.setBounds(openArea);
    
    // Sets the region for the play file button.
    Rectangle<int> playArea(getWidth()/10, getHeight()*4/15);
    playArea.setPosition(openArea.getBottomLeft());
    play.setBounds(playArea);
    
    // Sets the region for the pause file button.
    Rectangle<int> pauseArea(getWidth()/10, getHeight()*4/15);
    pauseArea.setPosition(playArea.getBottomLeft());
    pause.setBounds(pauseArea);
    
    // Sets the region for the stop file button.
    Rectangle<int> stopArea(getWidth()/10, getHeight()*4/15);
    stopArea.setPosition(pauseArea.getBottomLeft());
    stop.setBounds(stopArea);
    
    // Sets the region for the spectral view.
    Rectangle<int> spectrumArea(getWidth()*4/5, getHeight()*4/5);
    spectrumArea.setCentre(getLocalBounds().getCentre());
    spectrum.setBounds(spectrumArea);
    
    // Sets the region for the meter.
    Rectangle<int> meterArea(getWidth()/10, getHeight()*4/5);
    meterArea.setPosition(spectrumArea.getTopRight());
    meter.setBounds(meterArea);
    
    // Sets the region for the progess bar.
    Rectangle<int> progressBarArea(getWidth()*9/10,getHeight()/10);
    progressBarArea.setPosition(stopArea.getBottomLeft());
    transportProgress.setBounds(progressBarArea);
    
    // Sets the region for the volume adjustment box.
    Rectangle<int> textArea(getWidth()/10,getHeight()/10);
    textArea.setPosition(spectrumArea.getBottomRight());
    volumeBox.setBounds(textArea);

    
}


