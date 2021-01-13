/*
  ==============================================================================

    AudioEngine.cpp
    Created: 13 Jan 2021 2:27:08pm
    Author:  Julien Bloit

  ==============================================================================
*/

#include "AudioEngine.h"
AudioEngine::AudioEngine()
{
    initLink();
}
AudioEngine::~AudioEngine()
{
    if (link->isEnabled())
        link->enable(false);
}

#pragma mark - AudioSource

void AudioEngine::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {}
void AudioEngine::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) {}
void AudioEngine::releaseResources() {}


#pragma mark - Link

void AudioEngine::initLink()
{
    link.reset(new ableton::Link{ 60 });
    link->setTempoCallback([this](const double p) { DBG("TEMPO CHANGED TO " << juce::String(p)); });
    link->enable(true);
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
