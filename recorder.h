#ifndef RECORDER_H
#define RECORDER_H

#include <QAudioInput>
#include <QFile>
#include <QObject>

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent = nullptr);

    void setFilePath(const QString &value);

    void startRecord();
    void stopRecord();

private:
    QString filePath = "";
    QFile destinationFile;
    int simpleRate = 16000;
    int simpleSize = 16;
    int channelCount = 1;
    QString codeC = "audio/pcm";

    QAudioInput* m_audioInput = nullptr;

    int addWavHeader(QString catheFileName , QString wavFileName);

signals:

public slots:
};

#endif // RECORDER_H
