
#pragma once

#include <JuceHeader.h>

class AssetsManager
{
public:
    AssetsManager()
    {
        extractMidiFiles();
        indexAllMidiFiles();
        indexAllWavFiles();
    };

    ~AssetsManager()
    {
    };

    juce::File getMidiFile(int index)
    {
        jassert(index > -1);
        jassert(index < midiFiles.size());
        return midiFiles.getUnchecked(index);
    }

    int getNumMidiFiles(){
        return midiFiles.size();
    }

    int getNumWavFiles(){
        return wavSamples.size();
    }

    juce::String getSampleName(int index)
    {
        jassert(index > -1);
        jassert(index < wavSamples.size());
        return wavSamples.getUnchecked(index);
    }

    /** store all midi files found in assets dir */
    void indexAllMidiFiles()
    {
        midiFiles = getAssetsDir().findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.mid");
        midiFiles.sort();
    }

    void indexAllWavFiles()
    {

        for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
            auto filename = BinaryData::getNamedResourceOriginalFilename(BinaryData::namedResourceList[i]);

            auto fileString = juce::String(filename);
            if (fileString.matchesWildcard("*.wav", true)) {
                DBG("found wav sample " << fileString);
                wavSamples.add(fileString);
            }
        }
    }

private:

    juce::Array<juce::File> midiFiles;

    juce::Array<juce::String> wavSamples;


    void extractMidiFiles()
    {
        for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
            auto filename = BinaryData::getNamedResourceOriginalFilename(BinaryData::namedResourceList[i]);

            auto fileString = juce::String(filename);
            if (fileString.matchesWildcard("*.mid", true)) {
                DBG("found a midi file: copying it to the resources dir.");
                juce::File destFile = getAssetsDir().getChildFile(fileString);
                binaryDataToFile(fileString, destFile);
            }
        }
    }

    juce::File getAssetsDir()
    {
#if JUCE_IOS || JUCE_ANDROID
        auto assetsDir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory);

#else
        auto assetsDir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory);
        assetsDir = assetsDir.getChildFile("OmedocInC");
        if (!assetsDir.exists()) {
            assetsDir.createDirectory();
        }
#endif

        return assetsDir;
    }

    void binaryDataToFile(juce::String sampleFileName, juce::File destFile)
    {
        // find binary resource from filename
        const char *namedResourceForFile;
        for (int j = 0; j < BinaryData::namedResourceListSize; ++j) {
            if (sampleFileName.compare(BinaryData::getNamedResourceOriginalFilename(BinaryData::namedResourceList[j])) == 0) {
                namedResourceForFile = BinaryData::namedResourceList[j];
            }
        }
        // if this is hit, then the  file was not found.
        jassert (namedResourceForFile != nullptr);

        int dataSizeInBytes = 0;
        const char *data = BinaryData::getNamedResource(namedResourceForFile, dataSizeInBytes);

        // if you hit this, the provided file name was probably not found
        // in the BinaryData.
        jassert (data != nullptr);
        destFile.replaceWithData(data, (size_t) dataSizeInBytes);
    }

};