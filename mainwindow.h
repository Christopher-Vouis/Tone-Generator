#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include "noisemaker.h"
#include "wavevisualizer.h"

#include <QLabel>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    NoiseMaker* nm;
    QPushButton* playButton, *stopButton;
    QComboBox* waveTypeSelect, *depthSelect;
    std::string frequencyStr;
    QVariant waveType;
    QLabel *frequencyLabel,
    *hzLabel,
    *waveLabel,
    *depthLabel,
    *harmonicsLabel,
    *dutyLabel,
    *waveImageLabel,
    *volumeLabel;
    WaveVisualizer* visualizer;
    QSlider *frequencySlider, *dutySlider, *harmonicSlider, *ampSlider;
    QLineEdit *frequencyEdit;

    float frequency;

    bool IsValidFrequency(std::string toCheck);
    void InitControls();
    void InitLabels();

private slots:
    void PlayTone();
    void StopTone();
    void FrequencyChanged();
    void ChangeWaveType();
    void UpdatePixmap();
};
#endif // MAINWINDOW_H
