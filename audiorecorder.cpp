/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QAudioProbe>
#include <QAudioRecorder>
#include <QDir>
#include <QFileDialog>
#include <QMediaRecorder>
#include <QTextCodec>
#include <QMessageBox>

#include "audiorecorder.h"

#include "ui_audiorecorder.h"

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

AudioRecorder::AudioRecorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AudioRecorder),
    outputLocationSet(false)
{
    ui->setupUi(this);

    audioRecorder = new QAudioRecorder(this);
    probe = new QAudioProbe;
    probe->setSource(audioRecorder);

    //audio devices
    ui->audioDeviceBox->addItem(tr("Default"), QVariant(QString()));
    foreach (const QString &device, audioRecorder->audioInputs()) {
        ui->audioDeviceBox->addItem(device, QVariant(device));
    }

    //audio codecs
    ui->audioCodecBox->addItem(tr("audio/pcm"), QVariant(QString("audio/pcm")));
    foreach (const QString &codecName, audioRecorder->supportedAudioCodecs()) {
        ui->audioCodecBox->addItem(codecName, QVariant(codecName));
    }

    //containers
    ui->containerBox->addItem(tr("audio/x-wav"), QVariant(QString("audio/x-wav")));
    foreach (const QString &containerName, audioRecorder->supportedContainers()) {
        ui->containerBox->addItem(containerName, QVariant(containerName));
    }

    //sample rate
    ui->sampleRateBox->addItem(tr("16000"), QVariant(16000));
    foreach (int sampleRate, audioRecorder->supportedAudioSampleRates()) {
        ui->sampleRateBox->addItem(QString::number(sampleRate), QVariant(
                sampleRate));
    }

    //channels
    ui->channelsBox->addItem(tr("1"), QVariant(1));
    ui->channelsBox->addItem(QStringLiteral("1"), QVariant(1));
    ui->channelsBox->addItem(QStringLiteral("2"), QVariant(2));
    ui->channelsBox->addItem(QStringLiteral("4"), QVariant(4));

    //bitrates:
    ui->bitrateBox->addItem(tr("256000"), QVariant(256000));
    ui->bitrateBox->addItem(QStringLiteral("32000"), QVariant(32000));
    ui->bitrateBox->addItem(QStringLiteral("64000"), QVariant(64000));
    ui->bitrateBox->addItem(QStringLiteral("96000"), QVariant(96000));
    ui->bitrateBox->addItem(QStringLiteral("128000"), QVariant(128000));
    ui->bitrateBox->addItem(QStringLiteral("256000"), QVariant(256000));

    //中文支持
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);

//    connect(&effect,&QSoundEffect::statusChanged,this,&AudioRecorder::on_playerStatusChanged);

    QFile file("./sentences.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
    }

    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);
        qDebug()<< str;
        QStringList sen_pin = str.split("|");
        QString file = sen_pin[1];
        file.replace(QString("\n"), QString(""));
        m_sentences.append(sen_pin[0]);
        m_fileNames.append(file);
    }
    ui->labelSentence->setText(m_sentences.at(0));
}

AudioRecorder::~AudioRecorder()
{
    delete audioRecorder;
    delete probe;
}

void AudioRecorder::on_playerStatusChanged()
{
//    if(effect.status() == QSoundEffect::Status) {
//        ui->playButton->setText("Play");
//    }
}

void AudioRecorder::on_recordButton_clicked()
{
    m_recorder.setFilePath(ui->labelPath->text()+"\\"+m_fileNames.at(m_indexSentences));
    m_recorder.startRecord();
    ui->stopButton->setEnabled(true);
    ui->recordButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->playButton->setText("Play");
    effect.stop();
    effect.setSource(QUrl::fromLocalFile(""));
    ui->playButton->setEnabled(false);
}

void AudioRecorder::on_stopButton_clicked()
{
    m_recorder.stopRecord();
    ui->nextButton->setEnabled(true);
    ui->recordButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->playButton->setText("Play");
    ui->playButton->setEnabled(true);
}

void AudioRecorder::on_nextButton_clicked()
{
    ui->labelSentence->setText(m_sentences.at(++m_indexSentences));
    ui->playButton->setText("Play");
    effect.stop();
    effect.setSource(QUrl::fromLocalFile(""));
    ui->playButton->setEnabled(false);
    qDebug() << m_indexSentences  << m_sentences.length();
    if (m_indexSentences == m_sentences.length()-1) {
        QMessageBox::information(NULL,"Finished","Thank you!",QMessageBox::Ok,QMessageBox::Ok);
        m_indexSentences = -1;
    }
}

void AudioRecorder::on_outputButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    QString fileName = fileDialog->getExistingDirectory();
    ui->labelPath->setText(fileName);
}



void AudioRecorder::on_playButton_clicked()
{
    if(ui->playButton->text() == "Play") {
        effect.setSource(QUrl::fromLocalFile(ui->labelPath->text()+"\\"+m_fileNames.at(m_indexSentences)+".wav"));
        //循环播放
    //    effect.setLoopCount(QSoundEffect::Infinite);
        //设置音量，0-1
        effect.setVolume(1);
        effect.play();

        ui->playButton->setText("Stop Play");
    }
    else {
        effect.stop();
        effect.setSource(QUrl::fromLocalFile(""));
        ui->playButton->setText("Play");
    }
}
