
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

        if (midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message.isNoteOff())
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

void MidiPlayer::loadPattern(int index)
{
    std::unique_ptr<juce::FileInputStream> inputStream = assets->getMidiFile(index).createInputStream();
    if (inputStream->openedOk())
    {
        midiFile.readFrom(*inputStream.get());
        initMidiSequence();
    }
}


void MidiPlayer::seekStart(float offset)
{
    playheadInTicks = -offset;
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

void MidiPlayer::setTicksPerBuffer(float newValue)
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
                + eventPtr->message.getDescription()
                + "note number "
                + juce::String(eventPtr->message.getNoteNumber()));
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
