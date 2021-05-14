

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
    void seekStart(int startSample = 0);
    
    /** set the ticks samples corresponding to the next audio buffer to be played*/
    void setTicksRegionToPlay(int tickIn, int tickOut);
    void setTicksPerBuffer(float numTicks);
    
    int getTicksPerQuarterNote();
    
    void loadPattern(int index);

    int getDurationInTatums() {
        return durationInTatums;
    };

    /** called on audio thread if link just had a new phase tatum (beat subdivision).
     * returns true if this triggered a loop resturt;
     * */
    bool newTatumLoopCandidate(int samplePos);

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
    
    /** a beat subdivision to which we quantize the pattern's duration. */
    float tatum = 0.5;
    
    int ticksPerQuarterNote = -1;
    
    /** the duraton of the sequence, in number of tatums, ie grid beats. */
    int durationInTatums = 0;

    int elapsedTatums = 0;

    int durationInTicks = 0;
    
    /** playhead, in ticks */
    double playheadInTicks = 0;
    
    /** The provided midi files are of type 1, and have their note events on a given track index: */
    int trackId = 1;


    /** the buffer sample index at which the midi sequence should start playing. */
    int startSample = 0;

    void midiFileToBuffer(double fromTick, double toTick, int bufferOffset);

    int bufLen = 0;


};
