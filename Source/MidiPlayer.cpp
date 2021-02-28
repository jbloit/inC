
#include "MidiPlayer.h"

MidiPlayer::MidiPlayer ()
{

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
    
    midiBuffer.clear();
    
    
    int nextEventIndex  = midiFile.getTrack(trackId)->getNextIndexAtTime(playheadInTicks);
    double nextEventTime = midiFile.getTrack(trackId)->getEventTime(nextEventIndex);
//    auto nextEventTimeInSamples = nextEventTime * sampleRate;
    
    while (nextEventTime >= playheadInTicks
           &&
           nextEventTime <= playheadInTicks + ticksPerBuffer )
    {
        auto bufferOffset = nextEventTime - playheadInTicks;
        
        if (midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message.isNoteOn())
        {
            midiBuffer.addEvent(midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message, bufferOffset);
        }
        
        nextEventIndex++;
        nextEventTime = midiFile.getTrack(trackId)->getEventTime(nextEventIndex);
    }
    
    playheadInTicks += ticksPerBuffer;
    
    if (playheadInTicks >= durationInTicks)
    {
        playheadInTicks = 0;
    }
    
}

#pragma mark - API

void MidiPlayer::loadPattern(const char* patternNamedResource)
{
    DBG("midi player : load pattern " << BinaryData::getNamedResourceOriginalFilename(patternNamedResource));
    std::unique_ptr<juce::MemoryInputStream> inputStream;
    
    int dataSizeInBytes;
    auto data = BinaryData::getNamedResource (patternNamedResource, dataSizeInBytes );
    
    inputStream.reset(new juce::MemoryInputStream(data, dataSizeInBytes, false));
    
    midiFile.readFrom(*inputStream.get());
    
    initMidiSequence();

}


void MidiPlayer::seekStart()
{
    playheadInTicks = 0;
}

const juce::MidiBuffer& MidiPlayer::getBuffer()
{
    return midiBuffer;
}


void MidiPlayer::setTicksRegionToPlay(int newTickIn, int newTickOut)
{
    tickIn = newTickIn;
    tickOut = newTickOut;
}

void MidiPlayer::setTicksPerBuffer(int newValue)
{
    ticksPerBuffer = newValue;
}

int MidiPlayer::getTicksPerQuarterNote()
{
    return ticksPerQuarterNote;
}


void MidiPlayer::initMidiSequence()
{
    
    DBG("Found N events in track " << midiFile.getTrack(trackId)->getNumEvents());
    
    sequence = juce::MidiMessageSequence{*midiFile.getTrack(trackId)};
    
    ticksPerQuarterNote  =  midiFile.getTimeFormat();
    jassert(ticksPerQuarterNote > 0);

    
    // Get tatum and duration
    
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
                durationInTicks = durationInTatums * tatum * ticksPerQuarterNote;
                
                DBG("durationInTatums : " << juce::String(durationInTatums));
                
            }
        }
    }
    
    
    
}
