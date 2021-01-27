/*
 ==============================================================================
 
 ControlsPane.cpp
 Created: 13 Jan 2021 2:39:44pm
 Author:  Julien Bloit
 
 ==============================================================================
 */

#include <JuceHeader.h>
#include "ControlsPane.h"

//==============================================================================
ControlsPane::ControlsPane()
{
    
    addAndMakeVisible(playButton);
    playButton.setButtonText("PLAY");
    playButton.addListener(this);
    
    addAndMakeVisible(linkButton);
    linkButton.setButtonText("LINK");
    linkButton.addListener(this);
    linkButton.setClickingTogglesState(true);
    
    addAndMakeVisible(sliderBpm);
    sliderBpm.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    sliderBpm.addListener(this);
    sliderBpm.setRange(20.0, 220.0);
    
    
    startTimer(60);
}

ControlsPane::~ControlsPane()
{
    playButton.removeListener(this);
    linkButton.removeListener(this);
    sliderBpm.removeListener(this);
}

void ControlsPane::paint (juce::Graphics& g)
{
    
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
}

void ControlsPane::resized()
{
    auto area = getLocalBounds();
    auto buttonsArea = area.removeFromLeft(proportionOfWidth(0.5));
    auto buttonH = buttonsArea.getHeight() / 10.f;
    
    linkButton.setBounds(buttonsArea.removeFromTop(buttonH));
    playButton.setBounds(buttonsArea.removeFromTop(buttonH));
    sliderBpm.setBounds(buttonsArea.removeFromTop(buttonH));
    
}

void ControlsPane::timerCallback()
{
    sliderBpm.setValue(audio->getCurrentBpm());
    
    auto audioState = audio->getState();
    switch(audioState)
    {
        case AudioEngine::Stopped:
        {
            playButton.setButtonText("Play");
            break;
        }
        case AudioEngine::Playing:
        {
            playButton.setButtonText("Stop");
            break;
        }
            
        case AudioEngine::Armed:
        {
            playButton.setButtonText("armed...");
            break;
        }
    }
}

void ControlsPane::buttonClicked (juce::Button* button)
{
    if (button == &linkButton)
    {
        audio->enableLink(linkButton.getToggleState());
    }
    if (button == &playButton)
    {
        auto audioState = audio->getState();
        switch(audioState)
        {
            case AudioEngine::Stopped:
            {
                audio->shouldPlay.exchange(true);
                break;
            }
            case AudioEngine::Playing:
            {
                audio->shouldStop.exchange(true);
                break;
            }
                
            default:
            {
                audio->shouldStop.exchange(true);
                break;
            }
        }
        
        
    }
    
}

void ControlsPane::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &sliderBpm)
    {
        audio->setBpm(sliderBpm.getValue());
    }
}
void ControlsPane::sliderDragStarted (juce::Slider* slider) {}
void ControlsPane::sliderDragEnded (juce::Slider* slider) {}
