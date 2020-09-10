#include "wavevisualizer.h"

WaveVisualizer::WaveVisualizer()
{

}

WaveVisualizer::WaveVisualizer(NoiseMaker* inNoiseMaker)
{
    nm = inNoiseMaker;
    pixmap = QPixmap(1000,500);
    image = QImage(1000,500, QImage::Format_RGB888);
    image.fill(Qt::blue);
}

QPixmap WaveVisualizer::getPixmap()
{
    pixmap = QPixmap::fromImage(image);
    return pixmap;
}

void WaveVisualizer::DrawPoint(int xCoord, float sample)
{
    int sampleY = 250 - (sample * 100), currentY;
    int difference = 0;
    bool rising;

    if(xCoord > 0)
    {
        if(sampleY > lastY)
        {
            difference = sampleY - lastY;
            rising = true;
        }
        else
        {
            difference = lastY - sampleY;
            rising = false;
        }

        for(int i = 0; i < difference; ++i)
        {
            currentY = ( rising ? lastY + i  : lastY - i);

            if(i < difference / 2)
            {
                image.setPixelColor(xCoord - 1, currentY, Qt::white);
            }
            else
            {
                image.setPixelColor(xCoord, currentY, Qt::white);
            }
        }
    }

    lastY = sampleY;
    image.setPixelColor(xCoord, sampleY, Qt::white);
}

void WaveVisualizer::DrawWave()
{
    image.fill(Qt::blue);
    for(int i = 0; i < 1000; ++i)
    {
        DrawPoint(i, nm->GetCurrentWavePoint(i*timestep));
    }    
}

void WaveVisualizer::UpdateDrawing()
{
    DrawWave();
}
