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
                                    fileLength(1.0),
                                    sampleFreq(44100),
                                    buffer(nullptr),
                                    bufferSize(512),
                                    wasPaused(true)

{
    // Sets the size of the application.
    setSize (720, 480);

    // Specifies the number of input and output channels that we want to open.
    setAudioChannels (2, 2);
    
    // Makes all the buttons visible.
    addAndMakeVisible(&open);
    addAndMakeVisible(&play);
    addAndMakeVisible(&pause);
    addAndMakeVisible(&stop);
    
    // Sets what all the text buttons say.
    open.setButtonText("---> Open a File <---");
    play.setButtonText("Play");
    pause.setButtonText("Pause");
    stop.setButtonText("Stop");
    
    // Disable all the buttons at the start.
    play.setEnabled(false);
    pause.setEnabled(false);
    stop.setEnabled(false);
    
    // Sets the colors of the buttons.
    open.setColour(TextButton::buttonColourId, Colour(255,212,22));
    open.setColour(TextButton::textColourOffId, Colours::black);
    play.setColour(TextButton::buttonColourId, Colours::green);
    play.setColour(TextButton::textColourOffId, Colours::black);
    pause.setColour(TextButton::buttonColourId, Colour(255,212,22));
    pause.setColour(TextButton::textColourOffId, Colours::black);
    stop.setColour(TextButton::buttonColourId, Colours::red);
    stop.setColour(TextButton::textColourOffId, Colours::black);
    
    /*
       Provides the functionality for the buttons, i.e., once they are clicked
       their appropriate functions are called using a lambda function.
       open, play, and stop have been adapted from JUCE's tutorial "Build an
       audio player".
    */
    open.onClick  = [this] { openClicked(); };
    play.onClick  = [this] { playClicked();  wasPaused = false; };
    pause.onClick = [this] { pauseClicked(); wasPaused = true;  };
    stop.onClick  = [this] { stopClicked();  wasPaused = true;  };
    stop.addListener(this); // Allows the stop button to react to mouse clicks.
    
    // Allows the transport bar to be moved, without clicks in the audio.
    transportProgress.onDragStart = [this]
    {
        pauseClicked();
    };
    transportProgress.onDragEnd   = [this]
    {
        projectSource.setPosition(transportProgress.getValue()*fileLength/100);
        if( !wasPaused )
        {
            playClicked();
        }
    };

    // Makes the spectrum analyzer, meter, and slider (with marks) visible on the GUI.
    addAndMakeVisible(&spectrum);
    addAndMakeVisible(&meter);
    addAndMakeVisible(&transportProgress);
    addAndMakeVisible(&fiftyPer);
    addAndMakeVisible(&twentyFivePer);
    addAndMakeVisible(&seventyFivePer);

    // Sets the transport bar so it displays percent through the audio file.
    transportProgress.setRange(0, 100);
    transportProgress.setTextValueSuffix(" %");
    transportProgress.setNumDecimalPlacesToDisplay(0);
    
    // Prevents/enables specific functionality of the transport/slider bar.
    transportProgress.setTextBoxIsEditable(false);
    transportProgress.setSliderSnapsToMousePosition(true);

    // Customizes the display of the audio progress bar.
    transportProgress.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 72, 48);
    transportProgress.setColour(juce::Slider::backgroundColourId, Colours::darkgrey);
    transportProgress.setColour(juce::Slider::trackColourId, Colour(255,212,22));
    transportProgress.setColour(juce::Slider::thumbColourId, Colour(255,212,22));
    transportProgress.setColour(juce::Slider::textBoxBackgroundColourId, Colour(255,212,22));
    transportProgress.setColour(juce::Slider::textBoxTextColourId, Colours::black);
    
    // Customizes the transport bar label(s)
    fiftyPer.setColour(Label::textColourId, Colour(255,212,22));
    fiftyPer.setText("50", dontSendNotification);
    fiftyPer.setJustificationType(Justification::centred);
    twentyFivePer.setColour(Label::textColourId, Colour(255,212,22));
    twentyFivePer.setText("25", dontSendNotification);
    twentyFivePer.setJustificationType(Justification::left);
    seventyFivePer.setColour(Label::textColourId, Colour(255,212,22));
    seventyFivePer.setText("75", dontSendNotification);
    seventyFivePer.setJustificationType(Justification::right);
    
    startTimer(100); // Starts the internal timing for the project. Corresponds to 1000/100 = 10 frames/second.
    filetypeManager.registerBasicFormats(); // Allows the user to select standard audio filetypes.
    projectSource.addChangeListener(this); // Adds a listener for button presses.
}

MainComponent::~MainComponent()
{
    delete[] buffer;  // Dealloacates the memory the buffer stored audio data in.
    buffer = nullptr; // To prevent possible bad dereferencing.
    shutdownAudio();  // This shuts down the audio device and clears the audio source.
}

//==============================================================================================================
void MainComponent::openClicked()
{
    FileChooser findFile("Select an audio file to play.", File::nonexistent, ""); // Creates a file chooser object.
    
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
            
            play.setEnabled(true); // Activates the play button so that the user can press it and start the app.
            fileSource.reset(selectedFileSource.release());
        }
        open.setButtonText(selectedFile.getFileName()); // Displays the filename.
    }
}

void MainComponent::playClicked()
{
    pause.setToggleState(false, NotificationType::dontSendNotification);
    changeState(Starting); // Calls changeState() to begin playback. Needs changeCallbackListener() to be
                           // implemented and listening to function.
}

void MainComponent::pauseClicked()
{

    pause.setToggleState(true, NotificationType::dontSendNotification);
    changeState(Pausing); // Calls changeState() to begin pausing. Needs changeCallbackListener() to be
                          // implemented and listening to function.
}

void MainComponent::stopClicked()
{
    pause.setToggleState(false, NotificationType::dontSendNotification);
    changeState(Stopping); // Calls changeState() to begin stopping. Needs changeCallbackListener() to be
                           // implemented and listening to function.
}

/*
    Most of this function is given in JUCE's tutorial. I added the cases Paused and Pausing so my app can pause.
    Paused's implementation is virtually identical to Stopped except it doesn't reset the position to zero.
    Pausing and Stopping are differentiated by pause's toggle state.
*/
void MainComponent::changeState(playState newState)
{
    if(state != newState) // If the new state of the app is different, after being called back a source change
                          // causing a callback.
    {
        state = newState; // We set the new state.
        
        switch(state)
        {
            case Paused:
                // Disable/enable buttons so that play/pause/stop can't be pressed twice in a row avoiding
                // redundancy/allowing the user to see the current state.
                stop.setEnabled(true);
                pause.setEnabled(false);
                play.setEnabled(true);
                break;
            case Stopped:
                // Disable/enable buttons so that play/pause/stop can't be pressed twice in a row avoiding
                // redundancy/allowing the user to see the current state.
                stop.setEnabled(false);
                pause.setEnabled(false);
                play.setEnabled(true);
                projectSource.setPosition(0.0); // Sets the transport position to the start of the audio file.
                break;
            case Starting:
                play.setEnabled(false);
                projectSource.start(); // Activates the source callback.
                break;
            case Playing:
                stop.setEnabled(true);
                pause.setEnabled(true);
                break;
            case Pausing:
                projectSource.stop(); // Stops the app from playing back audio. Pausing is differentiated from
                                      // stopping during the callback where the toggle state of pause is determined.
                break;
            case Stopping:
                projectSource.stop(); // Stops the app from playing back audio. pause is not toggled so, the
                                      // audio is reset to zero.
                break;
        }
    }
}

void MainComponent::buttonClicked(Button* button)
{
    if( button == &stop )     // If stop was the button that was clicked.
    {
        changeState(Stopped); // Stops the audio.
    }
}

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if( source == &projectSource )      // If the source of the callback is the app's source audio file.
    {
        if( projectSource.isPlaying() ) // Checks to see if the app is playing.
        {
            changeState(Playing);       // If so changes the state to playing.
        }
        
        // If the app is not playing then it is stopped. If paused was toggled, then it should be paused,
        // else it should be stopped.
        else if( pause.getToggleState() ) // If pause was toggled.
        {
            changeState(Paused);
        }
        else // If pause was not toggle.
        {
            changeState(Stopped);
        }
    }
}

void MainComponent::timerCallback()
{
    if( !(transportProgress.isMouseButtonDown()) ) // If the transport is being dragged, don't update the percent
                                                   // of the way through the audio file.
    {
        double projectTime = projectSource.getCurrentPosition();
        transportProgress.setValue(100*projectTime/fileLength);
    }
    
    if( buffer ) // If the audio buffer has been filled with samples, calls the functions that draw the graphics.
    {
        meter.createPeak(buffer, bufferSize);
        spectrum.createPeaks(buffer, bufferSize);
    }
}


//==============================================================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    projectSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    buffer = new float[samplesPerBlockExpected]; // Allocates space for the samples to be obtained from the buffer.
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
    bufferSize = filledBuffer.numSamples;

    for( auto i = 0; i < bufferSize; ++i ) // Fills buffer with each sample in the buffer filled by the fileSource.
    {
        buffer[i] = bufferPtr[i];
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
    spectrum.repaint();         // Refreshes the graphics for the spectrum.
    meter.repaint();            // Refreshes the graphics for the meter.
}

void MainComponent::resized()
{
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
    Rectangle<int> meterArea(getWidth()/10, getHeight()*9/10);
    meterArea.setPosition(spectrumArea.getTopRight());
    meter.setBounds(meterArea);
    
    // Sets the region for the progess bar.
    Rectangle<int> progressBarArea(getWidth()*9/10,getHeight()/10);
    progressBarArea.setPosition(stopArea.getBottomLeft());
    transportProgress.setBounds(progressBarArea);
    
    // Sets the region for the 50% marker.
    Rectangle<int> fiftyArea(getWidth()/20,getHeight()/20);
    fiftyArea.setCentre(getWidth()/2,getHeight()*69/70);
    fiftyPer.setBounds(fiftyArea);
    
    // Sets the region for the 25% marker.
    Rectangle<int> twentyFiveArea(getWidth()/20,getHeight()/20);
    twentyFiveArea.setCentre(getWidth()*5/16,getHeight()*69/70);
    twentyFivePer.setBounds(twentyFiveArea);

    // Sets the region for the 75% marker.
    Rectangle<int> seventyFiveArea(getWidth()/20,getHeight()/20);
    seventyFiveArea.setCentre(getWidth()*11/16,getHeight()*69/70);
    seventyFivePer.setBounds(seventyFiveArea);
}
