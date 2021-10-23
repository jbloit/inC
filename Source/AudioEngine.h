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
        tone,
        sampler
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

    int getPatterDurationInTatums();

    /** stop all currently playing notes.
     * handy if a pattern changes while a note is still playing.
     * */
    void flushAllNotes();

    /** get bar phase, called from app message thread */
    float getAppPhase();
    
    void loadPattern(int index);

    int getPeersCount();

    /** if empty string, plays a tone */
    void setSound(juce::String sampleName = "");

    void shouldPlayClick(bool);
    
#pragma mark - AudioSource
    double sampleRate = 44100;
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
private:
    
#pragma mark - Synth

    /** the synth to play patterns with*/
    juce::Synthesiser synth;

    /** the synth to play metronome sound with */
    juce::Synthesiser metroSynth;

    void initSynth(juce::String sampleName);
    void initMetroSynth();

    // the polyphony we allow for the synth
    int numVoices = 6;

    void addSounds(juce::String sampleName, juce::Synthesiser& toSynth);

    /** midi buffer for playing metronome events */
    juce::MidiBuffer metroMidiBuffer;

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
    
    float sampleToTick(double sampleIndex, int ticksPerBeat);
    
    std::unique_ptr<ableton::Link> link;
    ableton::link::HostTimeFilter<ableton::link::platform::Clock> host_time_filter;
    std::unique_ptr<ableton::Link::SessionState> session;
    static constexpr double beat_length = 0.5;
    
    EngineData shared_engine_data, lock_free_engine_data;
    std::mutex engine_data_guard;
    
    std::chrono::microseconds output_time;
    std::uint64_t sample_time = 0;
    bool is_playing = false;

    double barPhase = 0;
    double prevBarPhase = 0;
    void playClick(const juce::AudioSourceChannelInfo& bufferToFill, int sampleIndex);
    std::atomic<bool> doPlayClick{false};
    
#pragma mark - midiplayer
    
    MidiPlayer midiPlayer;

    /**
     * Checks for quantum phase wrap.
     Returns the sample index on which the phase wrapped.
     Returns -1 if bar didnt wrap around.
     */
    int getBarPhaseWrapIndex(const double sample_rate, const double quantum, const int buffer_size);
    
    
    std::atomic<bool> midiSequencePlaying {false};
    
    /** */
    std::atomic<bool> requestMidiSequencePlay {false};
};
