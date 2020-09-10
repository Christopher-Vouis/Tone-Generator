#ifndef WAVEVISUALIZER_H
#define WAVEVISUALIZER_H

#include "noisemaker.h"

#include <QPixmap>

class WaveVisualizer : public QObject
{
private:
    Q_OBJECT

    QImage image;
    QPixmap pixmap;
    float timestep = 1.0f/44100;
    NoiseMaker* nm;
    int lastY;

public:
    WaveVisualizer();
    WaveVisualizer(NoiseMaker* inNoiseMaker);
    QPixmap getPixmap();
    void DrawPoint(int xCoord, float sample);
    void DrawWave();

   public slots:
   void UpdateDrawing();

};

#endif // WAVEVISUALIZER_H
