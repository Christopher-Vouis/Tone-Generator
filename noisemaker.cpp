#include "noisemaker.h"
#include <QtGui>
#include <QAudio>

NoiseMaker::NoiseMaker()
{
    format.setSampleRate(sampleRate);
    format.setChannelCount(channelCount);
    format.setSampleSize(SAMPLE_SIZE * 8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::Float);

    audioOut = new QAudioOutput(format, NULL);

    byteArray = new QByteArray();
    byteArray->resize(SAMPLE_SIZE * sampleRate);

    audioInput = new QBuffer(byteArray);
    audioInput->open(QIODevice::ReadOnly);
    start();
}

void NoiseMaker::Play(float fr, NoiseMaker::WaveType* wType)
{
    switch(*wType)
    {
    case Sin:
        sampFunc = &NoiseMaker::GetSinSample;
        break;

    case Square:
        sampFunc = &NoiseMaker::GetSquareSample;
        break;
    }

    totalTime = 0.0f;
    timeStep = 1 / sampleRate;
    frequency = fr / channelCount;
    playing = true;
    AddSamplesToBuffer();
    audioOut->start(audioInput);
    start();
}

void NoiseMaker::run()
{
    while(playing)
    {
        if(audioInput->pos() >= sampleRate * 4)
        {
            audioInput->seek(0);
            AddSamplesToBuffer();
        }
    }
    audioOut->stop();
}

float NoiseMaker::GetSinSample(float fr, float t)
{
    return sin(t * fr * 2.0f * 3.14159f);
}

float NoiseMaker::GetSquareSample(float fr, float t)
{
    float sample = sin(t * fr * 2.0f * 3.14159f);
    float result;

    for(int i = 1; i < squareDepth; ++i)
    {
        if( (sample + 1.0f) < ((1.0f/squareDepth) * i))
        {
            result = -1.0f + ((2.0f/squareDepth) * i);
            return 0.025f * result;
        }
    }

    return 0.025f;
}


void NoiseMaker::AddSamplesToBuffer()
{
    char *ptr;
    float sample;
    char b[4];

    for(int i = 0; i <= sampleRate; ++i)
    {
        sample = (this->*sampFunc)(frequency, totalTime);
        ptr = reinterpret_cast<char*>(&sample);

        b[0] = *ptr;
        b[1] = *(ptr + 1);
        b[2] = *(ptr + 2);
        b[3] = *(ptr + 3);

        (*byteArray)[4 * i] = b[0];
        (*byteArray)[4 * i + 1] = b[1];
        (*byteArray)[4 * i + 2] = b[2];
        (*byteArray)[4 * i + 3] = b[3];

        totalTime += timeStep;
    }
}

void NoiseMaker::Pause()
{
    playing = false;
}

void NoiseMaker::ChangeDepth(int value)
{
    squareDepth = value;
    if(playing)
        AddSamplesToBuffer();
}
