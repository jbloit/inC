
#include "MidiPlayer.h"

MidiPlayer::MidiPlayer ()
{
    currentPattern.clear();
    newPattern.clear();

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
    auto ticksOverflow = lastTickToRead - currentPattern.durationInTicks;

    if (ticksOverflow > 0)
        lastTickToRead -= ticksOverflow;

//    midiFileToBuffer(firstTickToRead, lastTickToRead, startSample);
    midiSequenceToBuffer(firstTickToRead, lastTickToRead);
    startSample = 0;

    if (ticksOverflow > 0)
    {
        playheadInTicks = ticksOverflow;

    } else
    {
        playheadInTicks += ticksPerBuffer;
    }
}

void MidiPlayer::midiSequenceToBuffer(double fromTick, double toTick)
{
    if (toTick < fromTick)
        return;

    int nextEventIndex  = currentPattern.sequence.getNextIndexAtTime(fromTick);
    double nextEventTime = currentPattern.sequence.getEventTime(nextEventIndex);

    while (nextEventTime >= fromTick
            &&
            nextEventTime <= toTick )
    {
        auto bufferWriteIndex = nextEventTime - playheadInTicks;

        if (currentPattern.sequence.getEventPointer(nextEventIndex)->message.isNoteOn())
        {
            int samplePos = bufferWriteIndex / bufLen * ticksPerBuffer;
            midiBuffer.addEvent(currentPattern.sequence.getEventPointer(nextEventIndex)->message, samplePos);
        }

        if (currentPattern.sequence.getEventPointer(nextEventIndex)->message.isNoteOff())
        {
            int samplePos = bufferWriteIndex / bufLen * ticksPerBuffer;
            midiBuffer.addEvent(currentPattern.sequence.getEventPointer(nextEventIndex)->message, samplePos);
        }

        nextEventIndex++;
        nextEventTime = currentPattern.sequence.getEventTime(nextEventIndex);
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

//    DBG("ELAPSED TATUMS  " << elapsedTatums);

    if (elapsedTatums >= currentPattern.durationInTatums - 1)
    {
//        DBG("LOOP now");
        if (! newPattern.isEmpty() )
        {
            currentPattern.copy(newPattern);
            newPattern.clear();
        }
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
    return currentPattern.ticksPerQuarterNote;
}


void MidiPlayer::initMidiSequence()
{
    
//    DBG("Found N events in track " << midiFile.getTrack(trackId)->getNumEvents());

    newPattern.clear();

    newPattern.sequence = juce::MidiMessageSequence{*midiFile.getTrack(trackId)};

    newPattern.ticksPerQuarterNote  =  midiFile.getTimeFormat();
    jassert(newPattern.ticksPerQuarterNote > 0);

    // Get tatum and duration
    
    auto numTracks = midiFile.getNumTracks();
//    DBG("NUM TRACKS : " << juce::String(numTracks));
    for (int trackIdx = 0; trackIdx<numTracks; trackIdx++){
        for (int i=0; i<midiFile.getTrack(trackIdx)->getNumEvents(); i++)
        {
//            DBG("event time stamp " << juce::String(midiFile.getTrack(trackIdx)->getEventTime(i)));
            auto track = midiFile.getTrack(trackIdx);
            auto eventPtr = track->getEventPointer(i);

//            DBG("midi message #"
//                + juce::String(i)
//                + " :"
//                + eventPtr->message.getDescription()
//                + "note number "
//                + juce::String(eventPtr->message.getNoteNumber()));
            
            if (eventPtr->message.isEndOfTrackMetaEvent())
            {
                // quantize the pattern's duration according to a given tatum (beat subdivision).
                auto endOfTrackTime = eventPtr->message.getTimeStamp();
                newPattern.durationInTatums = ceil(((float)endOfTrackTime / (float)newPattern.ticksPerQuarterNote) / tatum);
                newPattern.durationInTicks = newPattern.durationInTatums * tatum * newPattern.ticksPerQuarterNote;
                
//                DBG("durationInTatums : " << juce::String(durationInTatums));
                
            }
        }
    }

    if (currentPattern.isEmpty())
    {
        currentPattern.copy(newPattern);
        newPattern.clear();
    }

}
