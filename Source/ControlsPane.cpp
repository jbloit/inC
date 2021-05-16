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

    addAndMakeVisible(playClickButton);
    playClickButton.setButtonText("Click");
    playClickButton.setToggleState(false, juce::dontSendNotification);
    playClickButton.addListener(this);

    addAndMakeVisible(sliderBpm);
    sliderBpm.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    sliderBpm.addListener(this);
    sliderBpm.setRange(20.0, 220.0);
    
    addAndMakeVisible(phaseSlider);
    phaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    phaseSlider.setRange(0, 1);
    phaseSlider.setEnabled(false);

    addAndMakeVisible(peersCountLabel);

    addAndMakeVisible(soundMenu);
    soundMenu.addListener(this);
    initSoundMenu();
    
    addAndMakeVisible(patternMenu);
    patternMenu.addListener(this);
    initPatternMenu();

    addAndMakeVisible(patternDurationLabel);
    
    startTimer(60);
}

ControlsPane::~ControlsPane()
{
    playButton.removeListener(this);
    linkButton.removeListener(this);
    sliderBpm.removeListener(this);
    patternMenu.removeListener(this);
    soundMenu.removeListener(this);
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

    auto phaseSliderArea = buttonsArea.removeFromTop(buttonH);
    playClickButton.setBounds(phaseSliderArea.removeFromRight(phaseSliderArea.getWidth() * 0.2));
    phaseSlider.setBounds(phaseSliderArea);

    auto patternMenuArea = buttonsArea.removeFromTop(buttonH);
    patternDurationLabel.setBounds(patternMenuArea.removeFromRight(patternMenuArea.getWidth() * 0.2));
    patternMenu.setBounds(patternMenuArea);

    auto soundRadioButtonsArea = buttonsArea.removeFromTop(buttonH);
    auto numSounds = 4;
    auto buttonW = soundRadioButtonsArea.getWidth() / numSounds;
    soundMenu.setBounds(soundRadioButtonsArea);

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

    if (button == &playClickButton)
    {
        audio->shouldPlayClick(playClickButton.getToggleState());
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
        audio->flushAllNotes();
        auto selectedID = patternMenu.getSelectedId() - 1;
        loadPatternForComboItem(selectedID);
        auto s = juce::String(audio->getPatterDurationInTatums()) + " croches";
        patternDurationLabel.setText(s, juce::dontSendNotification);
        audio->requestStart();
    }

    if (combo == &soundMenu)
    {
        auto wavSampleIndex = soundMenu.getSelectedItemIndex();
        if (wavSampleIndex == 0)
        {
            // empty string will load sine wave synth
            audio->setSound("");
        } else
        {
            auto wavSampleName = assets->getSampleName(wavSampleIndex - 1);
            audio->setSound(wavSampleName);
        }

    }
}

#pragma mark - helpers

void ControlsPane::loadPatternForComboItem(int selectedID)
{

    if (selectedID >= 0)
    {
        audio->flushAllNotes();
        audio->loadPattern(selectedID);
    }
}

void ControlsPane::initPatternMenu()
{

    for (int i = 0; i < assets->getNumMidiFiles(); ++i)
    {
        auto file = assets->getMidiFile(i);
        patternMenu.addItem(file.getFileNameWithoutExtension(), i+1);
    }
    patternMenu.setSelectedId(1);

}

void ControlsPane::initSoundMenu()
{
    soundMenu.addItem("Tone", 1);
    for (int i = 0; i < assets->getNumWavFiles(); ++i)
    {
        soundMenu.addItem(assets->getSampleName(i), i+2);
    }
    soundMenu.setSelectedId(1);

}
