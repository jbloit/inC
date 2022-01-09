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
    linkButton.setColour(juce::TextButton::buttonOnColourId, widgetColour);
    linkButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    linkButton.setColour(juce::TextButton::buttonDown, widgetColour.darker(0.2));

    addAndMakeVisible(playClickButton);
    playClickButton.setButtonText("C metronome");
    playClickButton.setColour(juce::ToggleButton::tickColourId, widgetColour);
    playClickButton.setToggleState(false, juce::dontSendNotification);
    playClickButton.addListener(this);

    addAndMakeVisible(sliderBpm);
    sliderBpm.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    sliderBpm.addListener(this);
    sliderBpm.setColour(juce::Slider::trackColourId, widgetColour);
    sliderBpm.setRange(50, 320, 1);
    sliderBpm.setTextValueSuffix (" bpm");


    addAndMakeVisible(soundMenu);
    soundMenu.setColour(juce::ComboBox::arrowColourId, widgetColour);
    soundMenu.setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
    soundMenu.setColour(juce::ComboBox::buttonColourId, juce::Colours::black);
    soundMenu.addListener(this);
    initSoundMenu();
    
    addAndMakeVisible(patternMenu);
    patternMenu.setColour(juce::ComboBox::arrowColourId, widgetColour);
    patternMenu.setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
    patternMenu.setColour(juce::ComboBox::buttonColourId, juce::Colours::black);
    patternMenu.addListener(this);
    initPatternMenu();

    addAndMakeVisible(patternLabel);
    patternLabel.setText("Cellule", juce::dontSendNotification);

    addAndMakeVisible(soundsLabel);
    soundsLabel.setText(juce::CharPointer_UTF8 ("Sonorit\xc3\xa9"), juce::dontSendNotification);

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
    g.fillAll (juce::Colours::black);   // clear the background
}

void ControlsPane::resized()
{
    auto area = getLocalBounds();
    auto buttonsArea = area;
    auto numRows = 5.f;
    auto buttonH = buttonsArea.getHeight() / numRows;
    auto padding = buttonH / 10.f;

    auto row0Area =  buttonsArea.removeFromTop(buttonH);
    auto linkButtonArea = row0Area.removeFromLeft(row0Area.getWidth() / 3);
    linkButton.setBounds(linkButtonArea.reduced(padding));
    sliderBpm.setBounds(row0Area.reduced(padding));

    auto row1Area  = buttonsArea.removeFromTop(buttonH / 2);
    playClickButton.setBounds(row1Area.reduced(padding));


    auto row2area = buttonsArea.removeFromTop(buttonH);
    auto playPauseButtonArea = row2area;
    playPauseButtonArea.setWidth(buttonH);
    playPauseButtonArea.setCentre(row2area.getCentre());
    playPauseButton.setBounds(playPauseButtonArea.reduced(padding));

    auto row3area = buttonsArea.removeFromTop(buttonH);
    auto patternMenuArea = row3area.removeFromRight(row3area.getWidth() * 2 / 3);
    patternMenu.setBounds(patternMenuArea.reduced(padding));
    patternLabel.setBounds(row3area.reduced(padding));

    auto row4area = buttonsArea.removeFromTop(buttonH);
    auto soundMenuArea = row4area.removeFromRight(row4area.getWidth() * 2 / 3);
    soundMenu.setBounds(soundMenuArea.reduced(padding));
    soundsLabel.setBounds(row4area.reduced(padding));


}

void ControlsPane::timerCallback()
{
    sliderBpm.setValue(audio->getCurrentBpm() * 2);


    int numPeers = audio->getPeersCount();
    if (numPeers != prevPeersCount)
    {
        if (numPeers == 0)
        {
            linkButton.setButtonText("0\nJoueur");
        } else if (numPeers == 1)
        {
            linkButton.setButtonText("1\nJoueur");
        } else if (numPeers > 1 )
        {
            linkButton.setButtonText(juce::String(numPeers) + "\nJoueurs");
        }
    }
    prevPeersCount = numPeers;


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
        audio->setBpm(sliderBpm.getValue() / 2);
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
        audio->loadPattern(selectedID);
    }
}

void ControlsPane::initPatternMenu()
{

    for (int i = 0; i < assets->getNumMidiFiles(); ++i)
    {
        auto file = assets->getMidiFile(i);

        auto filename = file.getFileNameWithoutExtension();

        auto itemName = filename.fromLastOccurrenceOf("in_C_", false, true);

        patternMenu.addItem(itemName, i+1);
    }
    patternMenu.setSelectedId(1);

}

void ControlsPane::initSoundMenu()
{
    soundMenu.addItem(juce::CharPointer_UTF8 ("T\xc3\xa9l\xc3\xa9phone"), 1);
    for (int i = 0; i < assets->getNumWavFiles(); ++i)
    {
        auto samplename = assets->getSampleName(i);

        auto itemName = samplename.upToFirstOccurrenceOf(".", false, true);

        soundMenu.addItem(itemName, i+2);
    }

    soundMenu.setSelectedId(1);

}
