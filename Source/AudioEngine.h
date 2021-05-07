#pragma once

#include <JuceHeader.h>
#include <ableton/Link.hpp>
#include <ableton/link/HostTimeFilter.hpp>
#include "MidiPlayer.h"
#include "SinewaveSynth.h"
#include "NoisySineSynth.h"
#include "SamplerSynth.h"



class AudioEngine : public juce::AudioSource
{
    enum SynthType
    {
        sine,
        noisySine,
        samplerFlute
    };

public:
    AudioEngine();
    ~AudioEngine();

#pragma mark - API
    void enableLink(bool);
    void setBpm(double newBpm);
    double getCurrentBpm();
    
    void requestStart();
    void requestStop();

    /** stop all currently playing notes.
     * handy if a pattern changes while a note is still playing.
     * */
    void flushAllNotes();

    /** get bar phase, called from app message thread */
    float getAppPhase();
    
    void loadPattern(const char* patternNamedResource);

    int getPeersCount();

    void setClearSineSynth();
    void setNoisySineSynth();
    void setFluteSampler();

    void setSynthType(SynthType newType);
    
#pragma mark - AudioSource
    double sampleRate = 44100;
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
private:
    
#pragma mark - Synth
    
    juce::Synthesiser synth;
    
    void initSynth(SynthType type = SynthType::sine);

    // the polyphony we allow for the synth
    int numVoices = 6;

    SynthType currentSynthType = sine;

    void addFluteSounds();
    
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
    
    int sampleToTick(double sampleIndex, int ticksPerBeat);
    
    std::unique_ptr<ableton::Link> link;
    ableton::link::HostTimeFilter<ableton::link::platform::Clock> host_time_filter;
    std::unique_ptr<ableton::Link::SessionState> session;
    static constexpr double beat_length = 1.;
    
    EngineData shared_engine_data, lock_free_engine_data;
    std::mutex engine_data_guard;
    
    std::chrono::microseconds output_time;
    std::uint64_t sample_time = 0;
    bool is_playing = false;

    
#pragma mark - midiplayer
    
    MidiPlayer midiPlayer;
    
    /** Checks for quantum phase wrap.
     When one is found, sets the midiSequencePlaying flag to true, and the requestMidiSequencePlay to false. Returns the sample index on which the phase wrapped. */
    std::size_t triggerMidiSequence(const double sample_rate, const double quantum, const int buffer_size);
    
    
    std::atomic<bool> midiSequencePlaying {false};
    
    /** */
    std::atomic<bool> requestMidiSequencePlay {false};
};
