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
    
    addAndMakeVisible(stopButton);
    stopButton.setButtonText("STOP");
    stopButton.addListener(this);
    
    
    addAndMakeVisible(linkButton);
    linkButton.setButtonText("LINK");
    linkButton.addListener(this);
    linkButton.setClickingTogglesState(true);
    
    addAndMakeVisible(sliderBpm);
    sliderBpm.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    sliderBpm.addListener(this);
    sliderBpm.setRange(20.0, 220.0);
    
    addAndMakeVisible(phaseSlider);
    phaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    phaseSlider.setRange(0, 1);
    phaseSlider.setEnabled(false);

    addAndMakeVisible(peersCountLabel);

    addAndMakeVisible(clearSineSynthButton);
    clearSineSynthButton.setRadioGroupId(666);
    clearSineSynthButton.setClickingTogglesState(true);
    clearSineSynthButton.setButtonText("clear sine");
    clearSineSynthButton.addListener(this);
    
    addAndMakeVisible(noisySineSynthButton);
    noisySineSynthButton.setRadioGroupId(666);
    noisySineSynthButton.setClickingTogglesState(true);
    noisySineSynthButton.setButtonText("noisy sine");
    noisySineSynthButton.addListener(this);
    
    
    clearSineSynthButton.setState(juce::Button::ButtonState::buttonDown);

    
    addAndMakeVisible(patternMenu);
    patternMenu.addListener(this);
    initPatternMenu();
    
    startTimer(60);
}

ControlsPane::~ControlsPane()
{
    playButton.removeListener(this);
    linkButton.removeListener(this);
    sliderBpm.removeListener(this);
    patternMenu.removeListener(this);
    clearSineSynthButton.removeListener(this);
    noisySineSynthButton.removeListener(this);
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
    auto buttonsArea = area;
    auto buttonH = buttonsArea.getHeight() / 10.f;

    auto linkButtonArea =  buttonsArea.removeFromTop(buttonH);
    peersCountLabel.setBounds(linkButtonArea.removeFromRight(area.getWidth()/2));
    linkButton.setBounds(linkButtonArea);
    playButton.setBounds(buttonsArea.removeFromTop(buttonH));
    stopButton.setBounds(buttonsArea.removeFromTop(buttonH));
    sliderBpm.setBounds(buttonsArea.removeFromTop(buttonH));
    phaseSlider.setBounds(buttonsArea.removeFromTop(buttonH));
    patternMenu.setBounds(buttonsArea.removeFromTop(buttonH));

    auto soundRadioButtonsArea = buttonsArea.removeFromTop(buttonH);
    clearSineSynthButton.setBounds(soundRadioButtonsArea.removeFromLeft(soundRadioButtonsArea.getWidth()/2));
    noisySineSynthButton.setBounds(soundRadioButtonsArea);

}

void ControlsPane::timerCallback()
{
    sliderBpm.setValue(audio->getCurrentBpm());
    
    phaseSlider.setValue(audio->getAppPhase());

    peersCountLabel.setText("Connections : " + juce::String(audio->getPeersCount()), juce::dontSendNotification);

}

void ControlsPane::buttonClicked (juce::Button* button)
{
    if (button == &linkButton)
    {
        audio->enableLink(linkButton.getToggleState());
    }
    if (button == &playButton)
    {
        audio->requestStart();
    }
    if (button == &stopButton)
    {
        audio->requestStop();
    }

    if (button == &clearSineSynthButton)
    {
        audio->setClearSineSynth();
    }

    if (button == &noisySineSynthButton)
    {
        audio->setNoisySineSynth();
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

void ControlsPane::comboBoxChanged (juce::ComboBox* combo)
{
    if (combo == &patternMenu)
    {
        auto selectedID = patternMenu.getSelectedId() - 1;
        auto selectedName = BinaryData::namedResourceList[selectedID];
        audio->flushAllNotes();
        audio->loadPattern(selectedName);
        
    }
}

#pragma mark - helpers

void ControlsPane::initPatternMenu()
{
    for (int i = 0; i < BinaryData::namedResourceListSize; i++)
    {
        auto filename = juce::String(BinaryData::originalFilenames[i]);
        
        juce::File fileFromName = juce::File("./" + filename);
        
        if (fileFromName.getFileExtension().compare(".mid") == 0)
        {
            patternMenu.addItem(BinaryData::originalFilenames[i], i+1);
        }
    }
    
    patternMenu.setSelectedId(2);
}
