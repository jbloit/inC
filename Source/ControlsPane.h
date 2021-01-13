/*
  ==============================================================================

    ControlsPane.h
    Created: 13 Jan 2021 2:39:44pm
    Author:  Julien Bloit

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"

//==============================================================================
/*
*/
class ControlsPane  : public juce::Component, public juce::Timer, public juce::Button::Listener
{
public:
    ControlsPane();
    ~ControlsPane() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    
    juce::SharedResourcePointer<AudioEngine> audio;
    
    juce::TextButton linkButton;
    juce::TextButton playButton;
    
    void buttonClicked (juce::Button*) override;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlsPane)
};
