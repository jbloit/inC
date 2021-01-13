/*
  ==============================================================================

    SynthSource.h
    Created: 13 Jan 2021 2:26:42pm
    Author:  Julien Bloit

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SinewaveSynth.h"

class SynthSource : public juce::AudioSource
{
public:
    SynthSource ();

#pragma mark - API
    void initMidiSequence();
    
#pragma mark - AudioSource
    void prepareToPlay (int /*samplesPerBlockExpected*/, double newSampleRate) override;
    void releaseResources() override ;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
private:
    //==============================================================================
    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    juce::MidiMessageCollector midiCollector;

    // the synth itself!
    juce::Synthesiser synth;
    double sampleRate;
    juce::MidiMessageSequence sequence;
    juce::MidiBuffer midiBuffer;
    
    juce::MidiFile midiFile;
    
    double samplePosition = 0;
    
    
    
    void setUsingSineWaveSound();
    
};
