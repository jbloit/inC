

#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"



class PlayPauseButton : public juce::DrawableButton
{
public:
    PlayPauseButton(): juce::DrawableButton("playPause", ButtonStyle::ImageFitted)
    {
        play->replaceColour(juce::Colours::black, juce::Colours::white);
        pause->replaceColour(juce::Colours::black, juce::Colours::white);
        setImages(play.get(),play.get(), pause.get(), nullptr, pause.get(),pause.get(),pause.get(), nullptr);
        setColour(DrawableButton::ColourIds::backgroundColourId, juce::Colours::black);
        setColour(DrawableButton::ColourIds::backgroundOnColourId, juce::Colours::black);

    };
    ~PlayPauseButton(){};

    std::unique_ptr<juce::Drawable> play = juce::Drawable::createFromImageData(BinaryData::fadplay_svg, BinaryData::fadplay_svgSize);
    std::unique_ptr<juce::Drawable> pause = juce::Drawable::createFromImageData(BinaryData::fadpause_svg, BinaryData::fadpause_svgSize);
};


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
    int prevPeersCount = -1;
    PlayPauseButton     playPauseButton;

    juce::Slider        sliderBpm;
    juce::ToggleButton  playClickButton;

    juce::ComboBox       patternMenu;
    juce::Label         patternLabel;

    juce::ComboBox      soundMenu;
    juce::Label         soundsLabel;

    void buttonClicked (juce::Button*) override;
    void sliderValueChanged (juce::Slider*) override;
    void sliderDragStarted (juce::Slider*) override;
    void sliderDragEnded (juce::Slider*) override;
    void comboBoxChanged (juce::ComboBox*) override;

    juce::Colour widgetColour = juce::Colours::darkmagenta;
    
#pragma mark - helpers
    void loadPatternForComboItem(int comboIndex);
    void initPatternMenu();
    void initSoundMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlsPane)
};
