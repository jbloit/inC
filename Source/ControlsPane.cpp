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

    addAndMakeVisible(playPauseButton);
    playPauseButton.setClickingTogglesState(true);
    playPauseButton.addListener(this);
    
    
    addAndMakeVisible(linkButton);
    linkButton.addListener(this);
    linkButton.setClickingTogglesState(true);
    linkButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkorange);

    addAndMakeVisible(playClickButton);
    playClickButton.setButtonText("Click");
    playClickButton.setToggleState(false, juce::dontSendNotification);
    playClickButton.addListener(this);

    addAndMakeVisible(sliderBpm);
    sliderBpm.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    sliderBpm.addListener(this);
    sliderBpm.setRange(50, 160, 1);


    addAndMakeVisible(soundMenu);
    soundMenu.addListener(this);
    initSoundMenu();
    
    addAndMakeVisible(patternMenu);
    patternMenu.addListener(this);
    initPatternMenu();

    addAndMakeVisible(patternDurationLabel);
    
    startTimer(100);
}

ControlsPane::~ControlsPane()
{
    playPauseButton.removeListener(this);
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

    linkButton.setBounds(linkButtonArea);
    playPauseButton.setBounds(buttonsArea.removeFromTop(buttonH));
    sliderBpm.setBounds(buttonsArea.removeFromTop(buttonH));

    auto playClickButtonArea = buttonsArea.removeFromTop(buttonH);
    playClickButton.setBounds(playClickButtonArea);

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


    int numPeers = audio->getPeersCount();
    if (numPeers != prevPeersCount)
    {
        if (numPeers == 0)
        {
            linkButton.setButtonText("0 Connection");
        } else if (numPeers == 1)
        {
            linkButton.setButtonText("1 Connection");
        } else if (numPeers > 1 )
        {
            linkButton.setButtonText(juce::String(numPeers) + " Connections");
        }
    }
    prevPeersCount = numPeers;

    auto s = juce::String(audio->getPatterDurationInTatums()) + " croches";
    patternDurationLabel.setText(s, juce::dontSendNotification);

}

void ControlsPane::buttonClicked (juce::Button* button)
{
    if (button == &linkButton)
    {
        audio->enableLink(linkButton.getToggleState());
    }
    if (button == &playPauseButton)
    {
        if (playPauseButton.getToggleState())
            audio->requestStart();
        else
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

        auto selectedID = patternMenu.getSelectedId() - 1;
        loadPatternForComboItem(selectedID);

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
