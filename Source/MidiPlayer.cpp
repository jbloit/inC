
#include "MidiPlayer.h"

MidiPlayer::MidiPlayer ()
{
    initMidiSequence();
}



#pragma mark - AudioSource
void MidiPlayer::prepareToPlay (int /*samplesPerBlockExpected*/, double newSampleRate)
{
    
    sampleRate = newSampleRate;
    
    
}

void MidiPlayer::releaseResources()  {}

void MidiPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    
    bufferToFill.clearActiveBufferRegion();
    
    auto numSamples = bufferToFill.numSamples;
    midiBuffer.clear();
    
    
    int nextEventIndex  = midiFile.getTrack(trackId)->getNextIndexAtTime(samplePosition/sampleRate);
    double nextEventTime = midiFile.getTrack(trackId)->getEventTime(nextEventIndex);
    auto nextEventTimeInSamples = nextEventTime * sampleRate;
    
    while (nextEventTimeInSamples >= samplePosition && nextEventTimeInSamples <= samplePosition + numSamples )
    {
        auto bufferOffset = nextEventTimeInSamples - samplePosition;
        
        if (midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message.isNoteOn())
        {
            midiBuffer.addEvent(midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message, bufferOffset);
        }
        
        nextEventIndex++;
        nextEventTimeInSamples = midiFile.getTrack(trackId)->getEventTime(nextEventIndex) * sampleRate;
    }
    
    
    // and now get the synth to process the midi events and generate its output.

    
    
    samplePosition += numSamples;
    
    if (samplePosition >= midiFile.getTrack(trackId)->getEndTime()*sampleRate)
    {
        samplePosition = 0;
    }
}

#pragma mark - API

const juce::MidiBuffer& MidiPlayer::getBuffer()
{
    return midiBuffer;
}

void MidiPlayer::initMidiSequence()
{
    std::unique_ptr<juce::MemoryInputStream> inputStream;
    inputStream.reset(new juce::MemoryInputStream(BinaryData::in_C_3_mid, BinaryData::in_C_3_midSize, false));
    
    midiFile.readFrom(*inputStream.get());
    
//    midiFile.convertTimestampTicksToSeconds();
    
    DBG("Found N events in track " << midiFile.getTrack(trackId)->getNumEvents());
    
    sequence = juce::MidiMessageSequence{*midiFile.getTrack(trackId)};
    
    ticksPerQuarterNote  =  midiFile.getTimeFormat();
    jassert(ticksPerQuarterNote > 0);

    
    // get tatum and duration
    
    auto numTracks = midiFile.getNumTracks();
    DBG("NUM TRACKS : " << juce::String(numTracks));
    for (int trackIdx = 0; trackIdx<numTracks; trackIdx++){
        for (int i=0; i<midiFile.getTrack(trackIdx)->getNumEvents(); i++)
        {
            DBG("event time stamp " << juce::String(midiFile.getTrack(trackIdx)->getEventTime(i)));
            auto track = midiFile.getTrack(trackIdx);
            auto eventPtr = track->getEventPointer(i);

            DBG("midi message #"
                + juce::String(i)
                + " :"
                + eventPtr->message.getDescription());
            DBG("brk");
            
            if (eventPtr->message.isEndOfTrackMetaEvent())
            {
                auto endOfTrackTime = eventPtr->message.getTimeStamp();
                durationInTatums = ceil(((float)endOfTrackTime / (float)ticksPerQuarterNote) / tatum);

                DBG("durationInTatums : " << juce::String(durationInTatums));
                
            }
        }
    }
    
    
    
}
