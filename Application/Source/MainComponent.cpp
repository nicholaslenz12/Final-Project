/*
  ==============================================================================================================
 
    MainComponent.cpp
    Author:  Nicholas Lenz
 
  ==============================================================================================================
*/

#include "MainComponent.h"

//==============================================================================================================
MainComponent::MainComponent() : state(Stopped)
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
    open.setButtonText("Open File");
    play.setButtonText("Play");
    pause.setButtonText("Pause");
    stop.setButtonText("Stop");
    
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
    pause.onClick = [this] { pauseClicked(); };
    stop.onClick  = [this] { stopClicked(); };
    
    // Makes the spectrum analyzer visable.
    addAndMakeVisible(&spectrum);
    
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
            play.setEnabled(true); // Prepares the play button so that it can play.
            fileSource.reset(selectedFileSource.release());
        }
    }
}

void MainComponent::playClicked()
{
    //pause.setToggleState(false, NotificationType::dontSendNotification);
    //stop.setToggleState(false, NotificationType::dontSendNotification);
    changeState(Starting);
}

void MainComponent::pauseClicked()
{

    //pause.setToggleState(true, NotificationType::dontSendNotification);
    //stop.setToggleState(false, NotificationType::dontSendNotification);
    changeState(Pausing);
}

void MainComponent::stopClicked()
{
    //pause.setToggleState(false, NotificationType::dontSendNotification);
    //stop.setToggleState(true, NotificationType::dontSendNotification);
    changeState(Stopping);
}

void MainComponent::changeState(playState newState)
{
    if(state != newState)
    {
        state = newState;
        
        switch(state)
        {
            case Stopped:
                stop.setEnabled(false);
                pause.setEnabled(false);
                play.setEnabled(true);
                if(pause.getToggleState())
                {
                    projectSource.setPosition(projectSource.getCurrentPosition());
                }
                if(stop.getToggleState())
                {
                    projectSource.setPosition(0.0);
                }
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

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if( source == &projectSource ) // If the source of the callback is the projects source audio file.
    {
        if( projectSource.isPlaying() ) // Checks to see if the project is playing.
        {
            changeState(Playing); // If so changes the stat to playing.
        }
        else
        {
            changeState(Stopped); // If not changes it to stopped.
        }
    }
}



//==============================================================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    projectSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if( !fileSource ) // If there is no current file is loaded.
    {
        bufferToFill.clearActiveBufferRegion(); // Clear everyting so there's no noise.
    }
    else
    {
        projectSource.getNextAudioBlock(bufferToFill); // Fill the next audio block with the appropriate samples.
    }
}

void MainComponent::releaseResources()
{
    projectSource.releaseResources();
}

//==============================================================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black); // Makes the background black.
}

void MainComponent::resized()
{
    Rectangle<int> applicationArea = getLocalBounds(); // Total area of the GUI.
    
    // Sets the region for the spectral view.
    Rectangle<int> spectrumArea(getLocalBounds().getWidth()*4/5,getLocalBounds().getHeight()*4/5);
    spectrumArea.setCentre(getLocalBounds().getCentre());
    spectrum.setBounds(spectrumArea);

    // Sets the region for the open file button.
    Rectangle<int> openArea(getLocalBounds().getWidth(),getLocalBounds().getHeight()*1/10);
    openArea.setTop(getLocalBounds().getY());
    openArea.setLeft(getLocalBounds().getX());
    open.setBounds(openArea);
    
    // Sets the region for the play file button.
    Rectangle<int> playArea(getLocalBounds().getWidth()*1/10,getLocalBounds().getHeight()*4/15);
    playArea.setPosition(openArea.getBottomLeft());
    play.setBounds(playArea);
    
    // Sets the region for the pause file button.
    Rectangle<int> pauseArea(getLocalBounds().getWidth()*1/10,getLocalBounds().getHeight()*4/15);
    pauseArea.setPosition(playArea.getBottomLeft());
    pause.setBounds(pauseArea);
    
    
    // Sets the region for the stop file button.
    Rectangle<int> stopArea(getLocalBounds().getWidth()*1/10,getLocalBounds().getHeight()*4/15);
    stopArea.setPosition(pauseArea.getBottomLeft());
    stop.setBounds(stopArea);
}


