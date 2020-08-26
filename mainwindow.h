#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include "noisemaker.h"

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
    QLineEdit* frequencyInput;
    QComboBox* waveTypeSelect, *depthSelect;
    std::string freqencyStr;
    QVariant waveType;
    QLabel *frequencyLabel, *waveLabel, *depthLabel;

    bool IsValidFrequency(std::string toCheck);

private slots:
    void PlayTone();
    void StopTone();
};
#endif // MAINWINDOW_H
