

#pragma once
#include <JuceHeader.h>
#include "AssetsManager.h"

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
    
    /** reset playhead to the beginning*/
    void seekStart(float offset = 0);
    
    /** set the ticks samples corresponding to the next audio buffer to be played*/
    void setTicksRegionToPlay(int tickIn, int tickOut);
    void setTicksPerBuffer(float numTicks);
    
    int getTicksPerQuarterNote();
    
    void loadPattern(int index);
    
#pragma mark - AudioSource
    void prepareToPlay (int /*samplesPerBlockExpected*/, double newSampleRate) override;
    void releaseResources() override ;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
private:

    juce::SharedResourcePointer<AssetsManager> assets;

    double sampleRate;
    
    juce::MidiMessageSequence sequence;
    
    juce::MidiBuffer midiBuffer;
    
    juce::MidiFile midiFile;
    
    int tickIn = 0;
    int tickOut = 0;
    float ticksPerBuffer = 0;
    
    /** beat subdivision, on which a sequence can start */
    float tatum = 0.5;
    
    int ticksPerQuarterNote = -1;
    
    /** the duraton of the sequence, in number of tatums, ie grid beats. */
    int durationInTatums = 0;
    
    int durationInTicks = 0;
    
    /** playhead, in ticks */
    double playheadInTicks = 0;
    
    /** The provided midi files are of type 1, and have their note events on a given track index: */
    int trackId = 1;

};
