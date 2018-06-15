/*
  ==============================================================================================================
 
    MainComponent.cpp
    Author:  Nicholas Lenz
 
  ==============================================================================================================
*/

#include "MainComponent.h"

//==============================================================================================================
MainComponent::MainComponent()
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
    open.setButtonText("Open");
    play.setButtonText("Play");
    pause.setButtonText("Pause");
    stop.setButtonText("Stop");
    
    // Sets the colors of the buttons to make them look nicer.
    open.setColour(TextButton::buttonColourId, Colour(255,212,22));
    open.setColour(TextButton::textColourOffId, Colours::black);
    play.setColour(TextButton::buttonColourId, Colour(255,96,22));
    play.setColour(TextButton::textColourOffId, Colours::black);
    pause.setColour(TextButton::buttonColourId, Colour(255,96,22));
    pause.setColour(TextButton::textColourOffId, Colours::black);
    stop.setColour(TextButton::buttonColourId, Colour(255,96,22));
    stop.setColour(TextButton::textColourOffId, Colours::black);
    
    // Provides the functionality for the buttons, i.e. once they are clicked
    open.onClick = [this] { openClicked(); };
    
    // Makes the spectrum analyzer visable.
    addAndMakeVisible(&spectrum);
    
    filetypeManager.registerBasicFormats(); // Allows the user to select standard audio filetypes.
    //projectTime.addChangeListener(this); <======== Might need this later
    
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
            projectTime.setSource(selectedFileSource.get(), 0, nullptr, fileReader->sampleRate);
            play.setEnabled(true); // Prepares the play button so that it can play.
            fileSource.reset(selectedFileSource.release());
        }
    }
}

//==============================================================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);

    // You can add your drawing code here!
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
    Rectangle<int> playArea(getLocalBounds().getWidth()*1/10,getLocalBounds().getHeight()*1/10);
    playArea.setPosition(openArea.getBottomLeft());
    play.setBounds(playArea);
    
    // Sets the region for the pause file button.
    Rectangle<int> pauseArea(getLocalBounds().getWidth()*1/10,getLocalBounds().getHeight()*1/10);
    pauseArea.setPosition(playArea.getBottomLeft());
    pause.setBounds(pauseArea);
    
    
    // Sets the region for the stop file button.
    Rectangle<int> stopArea(getLocalBounds().getWidth()*1/10,getLocalBounds().getHeight()*1/10);
    stopArea.setPosition(pauseArea.getBottomLeft());
    stop.setBounds(stopArea);
}


