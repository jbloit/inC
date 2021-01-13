/*
  ==============================================================================

    AudioEngine.h
    Created: 13 Jan 2021 2:27:08pm
    Author:  Julien Bloit

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include <ableton/Link.hpp>
#include <ableton/link/HostTimeFilter.hpp>

class AudioEngine : public juce::AudioSource
{
public:
    AudioEngine();
    ~AudioEngine();
    
#pragma mark - API
    void enableLink(bool);
    
#pragma mark - AudioSource
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    
private:
    
    float currentBpm = 60;
    
#pragma mark - Link
    
    /** Data that's passed around between thread */
    struct EngineData
    {
        double requested_bpm;
        bool request_start;
        bool request_stop;
        double quantum;
        bool startstop_sync;
        JUCE_LEAK_DETECTOR(EngineData)
    };
    
    void initLink();
    
    void calculate_output_time(const double sample_rate, const int buffer_size);
    std::chrono::microseconds calculateTimeAtSample(const std::uint64_t sampleTime, const double sample_rate, const int buffer_size);
    EngineData pull_engine_data();
    void process_session_state(const EngineData& engine_data);
    
    std::unique_ptr<ableton::Link> link;
    ableton::link::HostTimeFilter<ableton::link::platform::Clock> host_time_filter;
    std::unique_ptr<ableton::Link::SessionState> session;
    
    EngineData shared_engine_data, lock_free_engine_data;
    std::mutex engine_data_guard;
    
    std::chrono::microseconds output_time;
    std::uint64_t sample_time = 0;
    bool is_playing = false;
    
};
