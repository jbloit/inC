

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
public juce::Slider::Listener,
public juce::ComboBox::Listener

{
public:
    ControlsPane();
    ~ControlsPane() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    
    juce::SharedResourcePointer<AudioEngine> audio;
    juce::SharedResourcePointer<AssetsManager> assets;

    juce::TextButton    linkButton;
    juce::Label         peersCountLabel;
    juce::TextButton    playButton;
    juce::TextButton    stopButton;
    juce::Slider        sliderBpm;
    juce::ToggleButton  playClickButton;

    juce::ComboBox       patternMenu;
    juce::Label          patternDurationLabel;

    juce::ComboBox      soundMenu;

    void buttonClicked (juce::Button*) override;
    void sliderValueChanged (juce::Slider*) override;
    void sliderDragStarted (juce::Slider*) override;
    void sliderDragEnded (juce::Slider*) override;
    void comboBoxChanged (juce::ComboBox*) override;
    
#pragma mark - helpers
    void loadPatternForComboItem(int comboIndex);
    void initPatternMenu();
    void initSoundMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlsPane)
};
