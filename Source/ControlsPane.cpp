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
    
    startTimer(60);
}

ControlsPane::~ControlsPane()
{
    playButton.removeListener(this);
    linkButton.removeListener(this);
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

}

void ControlsPane::timerCallback()
{
    
}

void ControlsPane::buttonClicked (juce::Button* button)
{
    if (button == &linkButton)
    {
        audio->enableLink(linkButton.getToggleState());
    }
    if (button == &playButton)
    {
        
        
    }

}
