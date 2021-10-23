#include "MainComponent.h"



//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible(controlsPane);
    
    addAndMakeVisible(versionLabel);
    juce::String versionString = "version " + juce::String(ProjectInfo::versionString) + "-" + juce::String(BUILD_NUMBER);
    versionLabel.setText(versionString, juce::dontSendNotification);

    setSize (800, 600);

    // Specify the number of input and output channels that we want to open
    setAudioChannels (0, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    audio->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    audio->getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
{
    audio->releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    auto controlsPaneArea = area.removeFromTop(proportionOfHeight(0.9));
    controlsPane.setBounds(controlsPaneArea.reduced(proportionOfWidth(0.1)));
    versionLabel.setBounds(area);
}


void MainComponenttimerCallback()
{}
