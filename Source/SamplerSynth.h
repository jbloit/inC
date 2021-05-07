#pragma once
#include <JuceHeader.h>

//struct SamplerFluteSound : public juce::SamplerSound
//{
//    SamplerFluteSound() {}
//
//    bool appliesToNote (int /*midiNoteNumber*/) override    { return true; }
//    bool appliesToChannel (int /*midiChannel*/) override    { return true; }
//
//
//
//    auto add_sound = [&](const String& file_name, const int note) {
//        const auto file = File{ samples_folder.getChildFile(file_name) };
//        if (file.existsAsFile()) {
//            auto note_range = BigInteger{};
//            note_range.setBit(note);
//            std::unique_ptr<AudioFormatReader> reader{ afm.createReaderFor(file) };
//            addSound(new SamplerSound(String{}, *reader, note_range, note, 0.01, 0.1,
//                    reader->lengthInSamples / reader->sampleRate));
//        }
//
//
//
//};