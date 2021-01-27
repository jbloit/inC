

#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"

//==============================================================================
/*
*/
class ControlsPane  :
public juce::Component,
public juce::Timer,
public juce::Button::Listener,
public juce::Slider::Listener

{
public:
    ControlsPane();
    ~ControlsPane() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    
    juce::SharedResourcePointer<AudioEngine> audio;
    
    juce::TextButton    linkButton;
    juce::TextButton    playButton;
    juce::TextButton    stopButton;
    juce::Slider        sliderBpm;
    juce::Label         phaseLabel;
    juce::Slider         phaseSlider;
    
    
    void buttonClicked (juce::Button*) override;
    void sliderValueChanged (juce::Slider*) override;
    void sliderDragStarted (juce::Slider*) override;
    void sliderDragEnded (juce::Slider*) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlsPane)
};
