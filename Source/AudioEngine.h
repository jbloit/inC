/*
  ==============================================================================

    AudioEngine.h
    Created: 13 Jan 2021 2:27:08pm
    Author:  Julien Bloit

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
class AudioEngine : public juce::AudioSource
{
public:
    AudioEngine();
    ~AudioEngine();
    
    // AudioSource
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
};
