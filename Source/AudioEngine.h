#pragma once
#include <JuceHeader.h>
#include <ableton/Link.hpp>
#include <ableton/link/HostTimeFilter.hpp>
#include "MidiPlayer.h"
#include "SinewaveSynth.h"

class AudioEngine : public juce::AudioSource
{
public:
    AudioEngine();
    ~AudioEngine();
    
    enum State
    {
        Armed,
        Stopped,
        Playing
    };
    
#pragma mark - API
    void enableLink(bool);
    void setBpm(double newBpm);
    double getCurrentBpm();
    
    // state update
    State getState(){return state;}
    std::atomic<bool> shouldPlay {false};
    std::atomic<bool> shouldStop {false};
    
    
#pragma mark - AudioSource
    double sampleRate = 44100;
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
private:

    // internal state
    State state = Stopped;
    
    /** update internal state, called from audio thread */
    void updateState();
    
    
#pragma mark - Synth
    
    juce::Synthesiser synth;
    
    void initSynth();
    
    void setUsingSineWaveSound();

    
#pragma mark - Link
    
   double currentBpm = 60;
    
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
    float barPhase = 0;
    float prevBarPhase = 0;
    
#pragma mark - midiplayer
    MidiPlayer midiPlayer;
};
