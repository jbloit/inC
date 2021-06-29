#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"
#include "ControlsPane.h"
#include "AssetsManager.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    

private:
    juce::SharedResourcePointer<AssetsManager> assets;
    ControlsPane controlsPane;
    juce::SharedResourcePointer<AudioEngine> audio;
    juce::Label versionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
