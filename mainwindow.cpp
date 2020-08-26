#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    nm = new NoiseMaker();

    playButton = new QPushButton("Play", this);
    playButton->move((this->width()/2) - playButton->width()/2, (this->height()/2) - playButton->height()/2);
    connect(playButton, SIGNAL(released()), this, SLOT (PlayTone()));

    stopButton = new QPushButton("Stop", this);
    stopButton->move((this->width()/2) - stopButton->width()/2, stopButton->height() + (this->height()/2) - stopButton->height()/2);
    connect(stopButton, SIGNAL(released()), this, SLOT (StopTone()));

    frequencyInput = new QLineEdit(this);
    frequencyInput->resize(playButton->size());
    frequencyInput->move(playButton->x(), playButton->y() - playButton->height());
    frequencyLabel = new QLabel("Frequency:", this);
    frequencyLabel->move(frequencyInput->x(), frequencyInput->y()-frequencyInput->height());

    waveTypeSelect = new QComboBox(this);
    waveTypeSelect->move(frequencyInput->x() - frequencyInput->width(), frequencyInput->y());
    waveTypeSelect->insertItem(0, "Sin", NoiseMaker::Sin);
    waveTypeSelect->insertItem(1, "Square", NoiseMaker::Square);
    waveLabel = new QLabel("Wave Type:", this);
    waveLabel->move(waveTypeSelect->x(), waveTypeSelect->y()-waveTypeSelect->height());

    depthSelect = new QComboBox(this);
    depthSelect->move(frequencyInput->x() + frequencyInput->width(), frequencyInput->y());
    depthSelect->insertItem(0, "2", 2);
    depthSelect->insertItem(1, "3", 3);
    depthSelect->insertItem(2, "4", 4);
    depthSelect->insertItem(3, "5", 5);
    depthSelect->insertItem(4, "6", 6);
    depthSelect->insertItem(5, "7", 7);
    depthSelect->insertItem(6, "8", 8);
    depthLabel = new QLabel("Square Depth:", this);
    depthLabel->move(depthSelect->x(), depthSelect->y()-depthSelect->height());

    connect(depthSelect, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](){ nm->ChangeDepth(depthSelect->currentData().toInt()); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::PlayTone()
{
    freqencyStr = std::string(frequencyInput->text().toUtf8());

    if(IsValidFrequency(freqencyStr))
    {
        playButton->setEnabled(false);
        float frequency;
        frequency = std::stof(freqencyStr);
        waveType = waveTypeSelect->currentData();
        nm->Play(frequency, reinterpret_cast<NoiseMaker::WaveType*>(&waveType));
    }
    else
    {
        QMessageBox alert;
        alert.setText("Invalid Frequency");
        alert.exec();
    }

}

void MainWindow::StopTone()
{
    playButton->setEnabled(true);
    nm->Pause();
}

bool MainWindow::IsValidFrequency(std::string toCheck)
{
    bool hasDecimal = false;

    if(toCheck.empty())
    {
        return false;
    }

    for(char c : toCheck)
    {
        if(c == '.')
        {
            if(!hasDecimal)
            {
                hasDecimal = true;
            }
            else
            {
               return false;
            }
        }
        else if(!isdigit(c))
        {
            return false;
        }
    }

    return true;
}
