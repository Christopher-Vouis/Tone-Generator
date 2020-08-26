#ifndef NOISEMAKER_H
#define NOISEMAKER_H

#include <QAudio>
#include <QAudioOutput>
#include <QBuffer>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QThread>
#include <QComboBox>

class NoiseMaker : public QThread
{
    Q_OBJECT

public:
    NoiseMaker();
    void AddSamplesToBuffer();
    void ChangeeSquareDepth(int value);
    enum WaveType{
        Sin,
        Square
    };


private:
    QAudioOutput* audioOut;
    QBuffer* audioInput;
    QByteArray* byteArray;
    QIODevice* ioDevice;
    QAudioFormat format;

    bool playing = false;

    float  sampleRate = 44100,
    timeStep,
    totalTime,
    frequency,
    dutyCycle;

    int squareDepth = 2,
    channelCount = 2;

    const int SAMPLE_SIZE = sizeof(float);

    WaveType waveType = Sin;

    void run() override;
    float GetSinSample(float fr, float t);
    float GetSquareSample(float fr, float t);

    typedef float (NoiseMaker::*SampleFunc)(float, float);

    SampleFunc sampFunc;


public slots:
    void Play(float fr, NoiseMaker::WaveType* wType);
    void Pause();
    void ChangeDepth(int value);

};

#endif // NOISEMAKER_H
