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

    byteArray = new QByteArray();
    byteArray->resize(SAMPLE_SIZE * sampleRate);

    audioOut = new QAudioOutput(format, NULL);
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
        bandLimitedSquare = false;
        highest = 0.0f;
        lowest = 0.0f;
        difference = 0.0f;
        step = 0.0f;
        harmonics = 2;
        break;

    case BandSquare:
        sampFunc = &NoiseMaker::GetSquareSample;
        bandLimitedSquare = true;
        highest = 0.0f;
        lowest = 0.0f;
        difference = 0.0f;
        step = 0.0f;
        break;

    case Sawtooth:
        sampFunc = &NoiseMaker::GetSawtoothSample;
        break;

    case BandSawtooth:
        sampFunc = &NoiseMaker::GetBandSawtoothSample;
        break;
    }

    totalTime = 0.0f;
    timeStep = 1.0f / sampleRate;
    frequency = fr / channelCount;
    AddSamplesToBuffer();
    audioInput->seek(0);

    if(!playing)
    {
        playing = true;
        audioOut->start(audioInput);
        start();
    }
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
    return sinf(t * fr * 2.0f * 3.14159f);
}

float NoiseMaker::GetSquareSample(float fr, float t)
{

    float wave1 = 0.0f,
            wave2 = 0.0f,
            lowWave1 = 0.0f,
            lowWave2 = 0.0f,
            highWave1 = 0.0f,
            highWave2 = 0.0f,
            offset = 0.0f,
            sum = 0.0f,
            phase = dutyCycle;


    float xValLow = (phase * 0.5f) / fr;
    float xValHigh = (phase + (0.5f * (1 - phase))) / fr;

    for(int i = 1; i < harmonics; ++i)
    {
        wave1 += sinf(t * fr * DOUBLE_PI * i) / i;
        wave2 += sinf((t * fr - phase) * DOUBLE_PI * i) / i;

        offset += sinf( -phase * DOUBLE_PI * i) / i;

        lowWave1 += sinf(xValLow * fr * DOUBLE_PI * i) / i;
        lowWave2 += sinf((xValLow  * fr - phase) * DOUBLE_PI * i) / i;

        highWave1 += sinf(xValHigh * fr * DOUBLE_PI * i) / i;
        highWave2 += sinf((xValHigh  * fr - phase) * DOUBLE_PI * i) / i;
    }


    sum = wave2 - wave1 - offset;

    if(bandLimitedSquare)
    {
        return sum;
    }

    lowest = lowWave2 - lowWave1 - offset;
    highest = highWave2 - highWave1 - offset;

    float result = floor((powf(2.0f, squareDepth) - 1.0f) * sum) / (powf(2.0f,squareDepth) - 1.0f);

    return result;
}

float NoiseMaker::GetBandSawtoothSample(float fr, float t)
{
    float sample = 0.0f;

    for(int i = 1; i <= harmonics; ++i)
    {
       sample += sinf(t * fr * DOUBLE_PI * i) / i;
    }

    return sample;
}

float NoiseMaker::GetSawtoothSample(float fr, float t)
{
    return -(2/3.14159f) * atanf( 1 / tanf( (t * 3.14159f) / (1/fr) ) );
}

void NoiseMaker::AddSamplesToBuffer()
{
    char *ptr;
    float sample;
    char b[4];

    for(int i = 0; i <= sampleRate; ++i)
    {
        sample = (this->*sampFunc)(frequency, totalTime) * amplitude;

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

void NoiseMaker::ChangeDuty(int dutyVal)
{
    dutyCycle = (dutyVal / 20.0f);    

    if(playing)
    {
        AddSamplesToBuffer();
        emit UpdateVisual();
    }
}

void NoiseMaker::Pause()
{
    playing = false;
}

void NoiseMaker::ChangeDepth(float value)
{
    squareDepth = value;

    if(playing)
    {
        AddSamplesToBuffer();
        emit UpdateVisual();
    }
}

void NoiseMaker::ChangeHarmonics(int harmonicValue)
{
    harmonics = harmonicValue;

    if(playing)
    {
        AddSamplesToBuffer();
        emit UpdateVisual();
    }
}

void NoiseMaker::ChangeAmplitude(int ampVal)
{
    amplitude = (float)ampVal / 100.0f;

    if(playing)
    {
        AddSamplesToBuffer();
        emit UpdateVisual();
    }
}

float NoiseMaker::GetCurrentWavePoint(float t)
{
    return (this->*sampFunc)(frequency, t);
}
