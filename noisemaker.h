#ifndef NOISEMAKER_H
#define NOISEMAKER_H

#include <QAudio>
#include <QAudioOutput>
#include <QBuffer>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QThread>
#include <QComboBox>
#include <math.h>

class NoiseMaker : public QThread
{
    Q_OBJECT

public:
    NoiseMaker();
    void AddSamplesToBuffer();
    void ChangeeSquareDepth(int value);
    float GetCurrentWavePoint(float t);
    enum WaveType{
        Sin,
        Square,
        BandSquare,
        Sawtooth,
        BandSawtooth
    };

    const float DOUBLE_PI = 2.0f * 3.14159f;

signals:
    void UpdateVisual();



private:
    QAudioOutput* audioOut;
    QBuffer* audioInput;
    QByteArray* byteArray;
    QIODevice* ioDevice;
    QAudioFormat format;

    bool playing = false,
    bandLimitedSquare = false;

    float  sampleRate = 44100,
    timeStep,
    totalTime,
    frequency,
    amplitude = 0.5f,
    dutyCycle = .05f,
    highest = 0.0f,
    lowest = 0.0f,
    highPoint = 0.0f,
    lowPoint = 0.0f,
    difference = 0.0f,
    squareDepth = 0.5f,
    step = 0.0f;

    int
    channelCount = 2,
    harmonics = 2;

    const int SAMPLE_SIZE = sizeof(float);

    WaveType waveType = Sin;

    void run() override;
    float GetSinSample(float fr, float t);
    float GetSquareSample(float fr, float t);
    float GetSawtoothSample(float fr, float t);
    float GetBandSawtoothSample(float fr, float t);

    typedef float (NoiseMaker::*SampleFunc)(float, float);

    SampleFunc sampFunc;


public slots:
    void Play(float fr, NoiseMaker::WaveType* wType);
    void Pause();
    void ChangeDepth(float value);
    void ChangeDuty(int dutyVal);
    void ChangeHarmonics(int harmonicValue);
    void ChangeAmplitude(int ampVal);

};

#endif // NOISEMAKER_H
