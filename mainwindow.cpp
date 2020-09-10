#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(1000,800);
    nm = new NoiseMaker();
    visualizer = new WaveVisualizer(nm);

    waveImageLabel = new QLabel(this);
    waveImageLabel->resize(1000,500);
    waveImageLabel->setPixmap(visualizer->getPixmap());
    waveImageLabel->move((this->width()/2)-(waveImageLabel->width()/2), 10);

    InitControls();
    InitLabels();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::PlayTone()
{
    frequencyStr = std::string(frequencyEdit->text().toUtf8());

    if(IsValidFrequency(frequencyStr))
    {
        frequency = std::stof(frequencyStr);
        waveType = waveTypeSelect->currentData();
        nm->Play(frequency, reinterpret_cast<NoiseMaker::WaveType*>(&waveType));
        visualizer->DrawWave();
        waveImageLabel->setPixmap(visualizer->getPixmap());
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

void MainWindow::FrequencyChanged()
{
    frequencyStr = std::string(frequencyEdit->text().toUtf8());

    if(IsValidFrequency(frequencyStr))
    {
        frequency = std::stof(frequencyStr);
    }

    float remainder = frequency - (int)frequency;
    frequency = frequencySlider->value() + remainder;
    frequencyEdit->setText(QString::number(frequency));
    PlayTone();
}

void MainWindow::ChangeWaveType()
{
    waveType = waveTypeSelect->currentData();
    depthSelect->setEnabled(false);
    harmonicSlider->setEnabled(false);
    dutySlider->setEnabled(false);

    switch(*reinterpret_cast<NoiseMaker::WaveType*>(&waveType))
    {
    case NoiseMaker::Sin:
        break;

    case NoiseMaker::Square:
        dutySlider->setEnabled(true);
        depthSelect->setEnabled(true);
        break;

    case NoiseMaker::BandSquare:
        dutySlider->setEnabled(true);
        harmonicSlider->setEnabled(true);
        break;

    case NoiseMaker::Sawtooth:
        break;

    case NoiseMaker::BandSawtooth:
        harmonicSlider->setEnabled(true);
        break;


    default:
        break;
    }   

    PlayTone();
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

void MainWindow::UpdatePixmap()
{
    visualizer->DrawWave();
    waveImageLabel->setPixmap(visualizer->getPixmap());
}

void MainWindow::InitControls()
{
    frequencySlider = new QSlider(Qt::Horizontal, this);
    frequencySlider->resize(800, frequencySlider->height());
    frequencySlider->move((this->width()/2) - (frequencySlider->width()/2), 575);
    frequencySlider->setMinimum(1);
    frequencySlider->setMaximum(1500);
    connect(frequencySlider, SIGNAL(valueChanged(int)), this, SLOT (FrequencyChanged()));

    waveTypeSelect = new QComboBox(this);
    waveTypeSelect->move(frequencySlider->x(), frequencySlider->y() + 75);
    waveTypeSelect->insertItem(0, "Sin", NoiseMaker::Sin);
    waveTypeSelect->insertItem(1, "Naive Square", NoiseMaker::Square);
    waveTypeSelect->insertItem(2, "Band-Limited Square", NoiseMaker::BandSquare);
    waveTypeSelect->insertItem(3, "Sawtooth", NoiseMaker::Sawtooth);
    waveTypeSelect->insertItem(4, "Band-Limited Sawtooth", NoiseMaker::BandSawtooth);
    connect(waveTypeSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeWaveType()));

    playButton = new QPushButton("Play", this);
    playButton->move((this->width()/2) - playButton->width()/2, waveTypeSelect->y());
    connect(playButton, SIGNAL(released()), this, SLOT (PlayTone()));

    stopButton = new QPushButton("Stop", this);
    stopButton->move((this->width()/2) - stopButton->width()/2, playButton->y() + playButton->height());
    connect(stopButton, SIGNAL(released()), this, SLOT (StopTone()));

    harmonicSlider = new QSlider(Qt::Horizontal, this);
    harmonicSlider->resize(800, harmonicSlider->height());
    harmonicSlider->move(frequencySlider->x(), stopButton->y() + stopButton->height() + 50);
    harmonicSlider->setMinimum(1);
    harmonicSlider->setMaximum(30);
    harmonicSlider->setEnabled(false);
    connect(harmonicSlider, SIGNAL(valueChanged(int)), nm, SLOT (ChangeHarmonics(int)));

    dutySlider = new QSlider(Qt::Horizontal, this);
    dutySlider->resize(800, dutySlider->height());
    dutySlider->move((this->width()/2) - (dutySlider->width()/2), harmonicSlider->y() + 100);
    dutySlider->setMinimum(1);
    dutySlider->setMaximum(18);
    dutySlider->setValue(10);
    dutySlider->setEnabled(false);
    connect(dutySlider, SIGNAL(valueChanged(int)), nm, SLOT (ChangeDuty(int)));

    frequencyEdit = new QLineEdit(this);
    frequencyEdit->move((this->width()/2) - (frequencyEdit->width()/2), frequencySlider->y()-frequencyEdit->height() - 10);
    frequencyEdit->setText("0");

    depthSelect = new QComboBox(this);
    depthSelect->move(frequencySlider->x() + frequencySlider->width(), waveTypeSelect->y());
    depthSelect->insertItem(0, "0.5", 0.5);
    depthSelect->insertItem(1, "1", 1);
    depthSelect->insertItem(2, "2", 2);
    depthSelect->insertItem(3, "3", 3);
    depthSelect->setEnabled(false);

    connect(depthSelect, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](){ nm->ChangeDepth(depthSelect->currentData().toFloat()); visualizer->DrawWave(); });

    ampSlider = new QSlider(this);
    ampSlider->resize(ampSlider->width() / 2, 300);
    ampSlider->move(waveImageLabel->x(), frequencySlider->y());
    ampSlider->setMinimum(0);
    ampSlider->setMaximum(100);
    ampSlider->setValue(50);
    connect(ampSlider, SIGNAL(valueChanged(int)), nm, SLOT (ChangeAmplitude(int)));

    connect(nm, SIGNAL(UpdateVisual()), this, SLOT(UpdatePixmap()));
}

void MainWindow::InitLabels()
{
    waveLabel = new QLabel("Wave Type:", this);
    waveLabel->move(waveTypeSelect->x(), waveTypeSelect->y()-waveTypeSelect->height());
    connect(waveTypeSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeWaveType()));

    harmonicsLabel = new QLabel("Harmonics:", this);
    harmonicsLabel->move(harmonicSlider->x(), harmonicSlider->y()-harmonicsLabel->height());

    dutyLabel = new QLabel("Duty Cycle:", this);
    dutyLabel->move(dutySlider->x(), dutySlider->y() - dutyLabel->height());

    frequencyLabel = new QLabel("Frequency:", this);
    frequencyLabel->move(frequencySlider->x(), frequencySlider->y()-frequencyLabel->height());

    depthLabel = new QLabel("Square Depth:", this);
    depthLabel->move(depthSelect->x(), waveLabel->y());

    connect(depthSelect, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](){ nm->ChangeDepth(depthSelect->currentData().toFloat()); visualizer->DrawWave(); });

    volumeLabel = new QLabel("Volume:", this);
    volumeLabel->move(ampSlider->x(), ampSlider->y() - volumeLabel->height());

    connect(nm, SIGNAL(UpdateVisual()), this, SLOT(UpdatePixmap()));
}
