/*
 ==============================================================================
 
 SynthSource.cpp
 Created: 13 Jan 2021 2:26:42pm
 Author:  Julien Bloit
 
 ==============================================================================
 */

#include "SynthSource.h"

SynthSource::SynthSource ()
{
    // Add some voices to our synth, to play the sounds..
    for (auto i = 0; i < 1; ++i)
    {
        synth.addVoice (new SineWaveVoice());   // These voices will play
    }
    
    // ..and add a sound for them to play...
    setUsingSineWaveSound();
    
}

void SynthSource::setUsingSineWaveSound()
{
    synth.clearSounds();
    synth.addSound (new SineWaveSound());
}


#pragma mark - AudioSource
void SynthSource::prepareToPlay (int /*samplesPerBlockExpected*/, double newSampleRate)
{
    
    sampleRate = newSampleRate;
    synth.setCurrentPlaybackSampleRate (sampleRate);
    initMidiSequence();
}

void SynthSource::releaseResources()  {}

void SynthSource::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    
    bufferToFill.clearActiveBufferRegion();
    
    auto numSamples = bufferToFill.numSamples;
    midiBuffer.clear();
    
    
    int nextEventIndex  = midiFile.getTrack(0)->getNextIndexAtTime(samplePosition/sampleRate);
    double nextEventTime = midiFile.getTrack(0)->getEventTime(nextEventIndex);
    auto nextEventTimeInSamples = nextEventTime * sampleRate;
    
    while (nextEventTimeInSamples >= samplePosition && nextEventTimeInSamples <= samplePosition + numSamples )
    {
        auto bufferOffset = nextEventTimeInSamples - samplePosition;
        
        if (midiFile.getTrack(0)->getEventPointer(nextEventIndex)->message.isNoteOn())
        {
            midiBuffer.addEvent(midiFile.getTrack(0)->getEventPointer(nextEventIndex)->message, bufferOffset);
        }
        
        nextEventIndex++;
        nextEventTimeInSamples = midiFile.getTrack(0)->getEventTime(nextEventIndex) * sampleRate;
    }
    
    
    // and now get the synth to process the midi events and generate its output.
    synth.renderNextBlock (*bufferToFill.buffer, midiBuffer, 0, bufferToFill.numSamples);
    
    
    samplePosition += numSamples;
    
    if (samplePosition >= midiFile.getTrack(0)->getEndTime()*sampleRate)
    {
        samplePosition = 0;
    }
}

#pragma mark - API

void SynthSource::initMidiSequence()
{
    std::unique_ptr<juce::MemoryInputStream> inputStream;
    inputStream.reset(new juce::MemoryInputStream(BinaryData::pianoPhase_mid, BinaryData::pianoPhase_midSize, false));
    
    midiFile.readFrom(*inputStream.get());
    
    midiFile.convertTimestampTicksToSeconds();
    
    DBG("Found N events " << midiFile.getTrack(0)->getNumEvents());
    
    sequence = juce::MidiMessageSequence{*midiFile.getTrack(0)};
    
    double sequenceEndTime = sequence.getEndTime();
    double fileEndTime = midiFile.getLastTimestamp();
    
    for (int i=0; i<midiFile.getTrack(0)->getNumEvents(); i++)
    {
        DBG("event time stamp " << juce::String(midiFile.getTrack(0)->getEventTime(i)));
    }
}
