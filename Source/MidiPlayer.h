

#pragma once
#include <JuceHeader.h>

/**
 Fill a midi buffer from midifile, according to current beat time.
 */
class MidiPlayer : public juce::AudioSource
{
public:
    MidiPlayer ();

#pragma mark - API
    
    const juce::MidiBuffer& getBuffer();
    
    void initMidiSequence();
    
#pragma mark - AudioSource
    void prepareToPlay (int /*samplesPerBlockExpected*/, double newSampleRate) override;
    void releaseResources() override ;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
private:

    double sampleRate;
    
    juce::MidiMessageSequence sequence;
    
    juce::MidiBuffer midiBuffer;
    
    juce::MidiFile midiFile;
    
    /** beat subdivision, on which a sequence can start */
    float tatum = 0.5;
    
    int ticksPerQuarterNote = -1;
    
    /** the duraton of the sequence, in number of tatums, ie grid beats. */
    int durationInTatums = 0;
    
    // TODO: replace with tick position
    double samplePosition = 0;
    
    /** The provided midi files are of type 1, and have their note events on a given track index: */
    int trackId = 1;

};
