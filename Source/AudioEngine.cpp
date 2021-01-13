/*
  ==============================================================================

    AudioEngine.cpp
    Created: 13 Jan 2021 2:27:08pm
    Author:  Julien Bloit

  ==============================================================================
*/

#include "AudioEngine.h"
AudioEngine::AudioEngine(){}
AudioEngine::~AudioEngine(){}

void AudioEngine::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {}
void AudioEngine::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) {}
void AudioEngine::releaseResources() {}
