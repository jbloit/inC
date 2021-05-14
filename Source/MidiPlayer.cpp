
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

    bufLen = bufferToFill.numSamples;

    bufferToFill.clearActiveBufferRegion();
    
    midiBuffer.clear();

    auto firstTickToRead = playheadInTicks;
    auto lastTickToRead = firstTickToRead + ticksPerBuffer;
    auto ticksOverflow = lastTickToRead - durationInTicks;

    if (ticksOverflow > 0)
        lastTickToRead -= ticksOverflow;

    midiFileToBuffer(firstTickToRead, lastTickToRead, startSample);
    startSample = 0;

    if (ticksOverflow > 0)
    {
        playheadInTicks = ticksOverflow;
//        midiFileToBuffer(0, ticksOverflow, startSample);

    } else
    {
        playheadInTicks += ticksPerBuffer;
    }
}

void MidiPlayer::midiFileToBuffer(double fromTick, double toTick, int startPositionInBuffer)
{

    if (toTick < fromTick)
        return;


    int nextEventIndex  = midiFile.getTrack(trackId)->getNextIndexAtTime(fromTick);
    double nextEventTime = midiFile.getTrack(trackId)->getEventTime(nextEventIndex);

    while (nextEventTime >= fromTick
            &&
            nextEventTime <= toTick )
    {
        auto bufferWriteIndex = nextEventTime - playheadInTicks;

        if (midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message.isNoteOn())
        {
            int samplePos = bufferWriteIndex / bufLen * ticksPerBuffer;
            midiBuffer.addEvent(midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message, samplePos);
        }

        if (midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message.isNoteOff())
        {
            int samplePos = bufferWriteIndex / bufLen * ticksPerBuffer;
            midiBuffer.addEvent(midiFile.getTrack(trackId)->getEventPointer(nextEventIndex)->message, samplePos);
        }

        nextEventIndex++;
        nextEventTime = midiFile.getTrack(trackId)->getEventTime(nextEventIndex);
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


 bool MidiPlayer::newTatumLoopCandidate(int samplePos)
{
    jassert(samplePos > -1);

    if (elapsedTatums >= durationInTatums - 1)
    {
        seekStart(samplePos);
        return true;
    }

    elapsedTatums++;
    return false;
}

void MidiPlayer::seekStart(int newStartSample)
{
    playheadInTicks = 0;
    elapsedTatums = 0;
    startSample = newStartSample;
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
                // quantize the pattern's duration according to a given tatum (beat subdivision).
                auto endOfTrackTime = eventPtr->message.getTimeStamp();
                durationInTatums = ceil(((float)endOfTrackTime / (float)ticksPerQuarterNote) / tatum);
                durationInTicks = durationInTatums * tatum * ticksPerQuarterNote;
                
                DBG("durationInTatums : " << juce::String(durationInTatums));
                
            }
        }
    }

//    juce::MidiMessageSequence longSeq(sequence);
//    for (int i = 0; i<50; ++i)
//    {
//        longSeq.addSequence(sequence, durationInTicks );
//        durationInTicks += durationInTicks;
//    }
//
//    sequence = longSeq;
    
    
    
}
