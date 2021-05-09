#include "AudioEngine.h"
AudioEngine::AudioEngine()
{
    initLink();
    initSynth();
}
AudioEngine::~AudioEngine()
{
    if (link->isEnabled())
        link->enable(false);
}

#pragma mark - API

void AudioEngine::setSynthType(SynthType newType)
{
    if (newType != currentSynthType)
    {
        currentSynthType = newType;
        initSynth(currentSynthType);
    }
}


void AudioEngine::loadPattern(int index)
{
    midiPlayer.loadPattern(index);
}

void AudioEngine::enableLink(bool shouldEnable)
{
    if (shouldEnable) DBG("LINK ON"); else DBG("LINK OFF");
    link->enable(shouldEnable);
}

void AudioEngine::setBpm(double newBpm)
{
    std::lock_guard<std::mutex> lock{ engine_data_guard };
    shared_engine_data.requested_bpm = newBpm;
}

double AudioEngine::getCurrentBpm()
{
    return currentBpm;
}


float AudioEngine::getAppPhase()
{
    const auto time = link->clock().micros();
    const auto app_session = link->captureAppSessionState();
    const auto phase = app_session.phaseAtTime(time, shared_engine_data.quantum);
    return phase;
}


void AudioEngine::requestStart()
{
    std::lock_guard<std::mutex> lock{ engine_data_guard };
    shared_engine_data.request_start = true;
    
    requestMidiSequencePlay.exchange(true);
}

void AudioEngine::requestStop()
{
    flushAllNotes();
    std::lock_guard<std::mutex> lock{ engine_data_guard };
    shared_engine_data.request_stop = true;
}

void AudioEngine::flushAllNotes()
{
    synth.allNotesOff(0, true);
}

int AudioEngine::getPeersCount()
{
    if (link != nullptr)
        return link->numPeers();

    return 0;
}

void AudioEngine::shouldPlayClick(bool flag)
{
   doPlayClick.exchange(flag);
}

void AudioEngine::setClearSineSynth()
{
    initSynth(SynthType::sine);
}

void AudioEngine::setNoisySineSynth()
{
    initSynth(SynthType::noisySine);
}

void AudioEngine::setFluteSampler()
{
    initSynth(SynthType::samplerFlute);
}

void AudioEngine::setGuitarSampler()
{
    initSynth(SynthType::samplerGuitar);
}

void AudioEngine::setAccordionSampler()
{
    initSynth(SynthType::samplerAccordion);
}

#pragma mark - AudioSource

void AudioEngine::prepareToPlay (int samplesPerBlockExpected, double newSampleRate) {
    sampleRate = newSampleRate;
    
    midiPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
    synth.setCurrentPlaybackSampleRate (sampleRate);
}
void AudioEngine::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{

    bufferToFill.clearActiveBufferRegion();
    
    calculate_output_time(sampleRate, bufferToFill.numSamples);

    // Extract info from link and modify its state as per user requests.
    const auto engine_data = pull_engine_data();
    process_session_state(engine_data);

    // did we just start a new bar?
    auto wrapIndex = getBarPhaseWrapIndex(sampleRate, engine_data.quantum, bufferToFill.numSamples);

    // Check whether the midi sequence needs to be launched
    if(requestMidiSequencePlay.load())
    {
        midiSequencePlaying.exchange(false);

        if (wrapIndex >= 0)
        {
            midiSequencePlaying.exchange(true);
            requestMidiSequencePlay.exchange(false);
        }

        if (midiSequencePlaying.load())
        {
            // reset MIDI sequence playhead
            midiPlayer.seekStart();
        }

    }



    // play a synth with its midi file
    if (is_playing && midiSequencePlaying.load())
    {
        
        auto tickIn = sampleToTick(sample_time, midiPlayer.getTicksPerQuarterNote());
        auto tickOut = sampleToTick(sample_time + bufferToFill.numSamples - 1, midiPlayer.getTicksPerQuarterNote());

        midiPlayer.setTicksPerBuffer(tickOut - tickIn);

        midiPlayer.getNextAudioBlock(bufferToFill);

        synth.renderNextBlock (*bufferToFill.buffer, midiPlayer.getBuffer(), 0, bufferToFill.numSamples);
    }

    sample_time += bufferToFill.numSamples;

    if (doPlayClick.load())
        playClick(bufferToFill, wrapIndex);
    
}


void AudioEngine::playClick(const juce::AudioSourceChannelInfo& bufferToFill, int sampleIndex)
{
    if (sampleIndex >= 0)
    {
        for(int chan = 0; chan<bufferToFill.buffer->getNumChannels(); ++chan)
        {
            bufferToFill.buffer->setSample(chan, sampleIndex, 1.0);
        }
    }


}


void AudioEngine::releaseResources()
{
    midiPlayer.releaseResources();

}

#pragma mark - Synth
void AudioEngine::initSynth(SynthType synthType)
{
    synth.clearSounds();
    synth.clearVoices();

    switch (synthType)
    {
        case SynthType::sine:
        {
            // Add some voices to our synth, to play the sounds..
            for (auto i = 0; i < numVoices; ++i)
            {
                synth.addVoice (new SineWaveVoice());   // These voices will play
            }
            // ..and add a sound for them to play...
            synth.addSound (new SineWaveSound());
            break;
        }

        case SynthType::noisySine:
        {
            // Add some voices to our synth, to play the sounds..
            for (auto i = 0; i < numVoices; ++i)
            {
                synth.addVoice (new NoisySineVoice());   // These voices will play
            }
            // ..and add a sound for them to play...
            synth.addSound (new NoisySineSound());
            break;
        }

        case SynthType::samplerFlute:
        {
            // Add some voices to our synth, to play the sounds..
            for (auto i = 0; i < numVoices; ++i)
            {
                synth.addVoice (new juce::SamplerVoice());   // These voices will play
            }
            // ..and add a sound for them to play...
            addFluteSounds();
            break;
        }

        case SynthType::samplerGuitar:
        {
            // Add some voices to our synth, to play the sounds..
            for (auto i = 0; i < numVoices; ++i)
            {
                synth.addVoice (new juce::SamplerVoice());   // These voices will play
            }
            // ..and add a sound for them to play...
            addGuitarSounds();
            break;
        }

        case SynthType::samplerAccordion:
        {
            // Add some voices to our synth, to play the sounds..
            for (auto i = 0; i < numVoices; ++i)
            {
                synth.addVoice (new juce::SamplerVoice());   // These voices will play
            }
            // ..and add a sound for them to play...
            addAccordionSounds();
            break;
        }
    }

}

void AudioEngine::addFluteSounds()
{
    juce::AudioFormatManager afm;
    afm.registerBasicFormats();

    juce::WavAudioFormat wavFormat;
    juce::AiffAudioFormat aifFormat;

    std::unique_ptr<juce::AudioFormatReader> reader (aifFormat.createReaderFor (
            new juce::MemoryInputStream (
                    BinaryData::FlordC4mf_aif ,
                    BinaryData::FlordC4mf_aifSize, false), true));


    if (reader.get() != nullptr)
    {
        auto noteRange = juce::BigInteger{};
        for (int i = 0; i< 128; ++i)
        {
            noteRange.setBit(i);
        }

        synth.addSound(new juce::SamplerSound("fluteC4", *reader.get(), noteRange, 60, 0.01, 0.1,
                reader->lengthInSamples / reader->sampleRate));
    }
}

void AudioEngine::addGuitarSounds()
{
    juce::AudioFormatManager afm;
    afm.registerBasicFormats();

    juce::WavAudioFormat wavFormat;
    juce::AiffAudioFormat aifFormat;

    std::unique_ptr<juce::AudioFormatReader> reader (aifFormat.createReaderFor (
            new juce::MemoryInputStream (
                    BinaryData::GtrharmfngrB4p2c_aif ,
                    BinaryData::GtrharmfngrB4p2c_aifSize, false), true));


    if (reader.get() != nullptr)
    {
        auto noteRange = juce::BigInteger{};
        for (int i = 0; i< 128; ++i)
        {
            noteRange.setBit(i);
        }

        synth.addSound(new juce::SamplerSound("guit", *reader.get(), noteRange, 59, 0.01, 0.1,
                reader->lengthInSamples / reader->sampleRate));
    }
}

void AudioEngine::addAccordionSounds()
{
    juce::AudioFormatManager afm;
    afm.registerBasicFormats();

    juce::WavAudioFormat wavFormat;
    juce::AiffAudioFormat aifFormat;

    std::unique_ptr<juce::AudioFormatReader> reader (aifFormat.createReaderFor (
            new juce::MemoryInputStream (
                    BinaryData::AccsfzB4fp_aif ,
                    BinaryData::AccsfzB4fp_aifSize, false), true));


    if (reader.get() != nullptr)
    {
        auto noteRange = juce::BigInteger{};
        for (int i = 0; i< 128; ++i)
        {
            noteRange.setBit(i);
        }

        synth.addSound(new juce::SamplerSound("acc", *reader.get(), noteRange, 59, 0.01, 0.1,
                reader->lengthInSamples / reader->sampleRate));
    }
}

#pragma mark - Link

void AudioEngine::initLink()
{
    
    shared_engine_data = EngineData{ 0., false, false, 1., false };
    lock_free_engine_data = EngineData{ shared_engine_data };
    
    link.reset(new ableton::Link{ currentBpm });
    link->setTempoCallback([this](const double newBpm) { currentBpm = newBpm; });

}

void AudioEngine::calculate_output_time(const double sample_rate, const int buffer_size)
{
    // Synchronize host time to reference the point when its output reaches the speaker.
    const auto host_time =  host_time_filter.sampleTimeToHostTime(sample_time);
    const auto output_latency = std::chrono::microseconds{ std::llround(1.0e6 * buffer_size / sample_rate) };
    output_time = output_latency + host_time;
}

std::chrono::microseconds AudioEngine::calculateTimeAtSample(const std::uint64_t sampleTime, const double sample_rate, const int buffer_size)
{
    const auto host_time =  host_time_filter.sampleTimeToHostTime(sampleTime);
    const auto output_latency = std::chrono::microseconds{ std::llround(1.0e6 * buffer_size / sample_rate) };
    return output_latency + host_time;
}


AudioEngine::EngineData AudioEngine::pull_engine_data()
{   // Safely operate on data isolated from user changes.
    auto engine_data = EngineData{};
    if (engine_data_guard.try_lock()) {
        engine_data.requested_bpm = shared_engine_data.requested_bpm;
        shared_engine_data.requested_bpm = 0;
        
        engine_data.request_start = shared_engine_data.request_start;
        shared_engine_data.request_start = false;
        
        engine_data.request_stop = shared_engine_data.request_stop;
        shared_engine_data.request_stop = false;
        
        lock_free_engine_data.quantum = shared_engine_data.quantum;
        lock_free_engine_data.startstop_sync = shared_engine_data.startstop_sync;
        
        engine_data_guard.unlock();
    }
    else
        DBG("entry failed");
    engine_data.quantum = lock_free_engine_data.quantum;
    return engine_data;
}


void AudioEngine::process_session_state(const EngineData& engine_data)
{
    session = std::make_unique<ableton::Link::SessionState>(link->captureAudioSessionState());

    if (engine_data.request_start)
        session->setIsPlaying(true, output_time);

    if (engine_data.request_stop)
        session->setIsPlaying(false, output_time);

    if (!is_playing && session->isPlaying()) {   // Reset the timeline so that beat 0 corresponds to the time when transport starts
        session->requestBeatAtTime(0., output_time, engine_data.quantum);
        is_playing = true;
    }
    else if (is_playing && !session->isPlaying())
        is_playing = false;

    if (engine_data.requested_bpm > 0) // Set the newly requested tempo from the beginning of this buffer.
        session->setTempo(engine_data.requested_bpm, output_time);

    link->commitAudioSessionState(*session); // Timeline modifications are complete, commit the results
}


int AudioEngine::sampleToTick(double sampleIndex, int ticksPerBeat)
{
    session = std::make_unique<ableton::Link::SessionState>(link->captureAudioSessionState());
    const auto micros_per_sample = 1.0e6 / sampleRate;
    const auto sampleTime = output_time + std::chrono::microseconds(llround(sampleIndex * micros_per_sample));
    auto beatAtTime = session->beatAtTime(sampleTime, 1.0);
    
    return (beatAtTime * ticksPerBeat);
}


#pragma mark - helpers

int AudioEngine::getBarPhaseWrapIndex(const double sample_rate, const double quantum, const int buffer_size)
{   // Taken from Ableton's linkhut example found on their github.
    const auto micros_per_sample = 1.0e6 / sample_rate;
    for (int i = 0; i < buffer_size; ++i) {
        // Compute the host time for this sample and the last.
        const auto host_time = output_time + std::chrono::microseconds(llround(i * micros_per_sample));
        const auto prev_host_time = host_time - std::chrono::microseconds(llround(micros_per_sample));
        
        // Only make sound for positive beat magnitudes. Negative beat
        // magnitudes are count-in beats.
        if (session->beatAtTime(host_time, quantum) >= 0.) {
            
            // If the phase wraps around between the last sample and the
            // current one with respect to a 1 beat quantum, then a sample trigger
            // should occur.
            if (session->phaseAtTime(host_time, beat_length)
                < session->phaseAtTime(prev_host_time, beat_length))
            {
                return i;
            }
        }
    }
    
    return -1;
}
