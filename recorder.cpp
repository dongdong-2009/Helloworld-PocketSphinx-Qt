#include "recorder.h"
#include <QDebug>
#include <QAudioDeviceInfo>
struct WAVFILEHEADER
{
    // RIFF 头;
    char RiffName[4];
    unsigned long nRiffLength;

    // 数据类型标识符;
    char WavName[4];

    // 格式块中的块头;
    char FmtName[4];
    unsigned long nFmtLength;

    // 格式块中的块数据;
    unsigned short nAudioFormat;
    unsigned short nChannleNumber;
    unsigned long nSampleRate;
    unsigned long nBytesPerSecond;
    unsigned short nBytesPerSample;
    unsigned short nBitsPerSample;

    // 数据块中的块头;
    char    DATANAME[4];
    unsigned long   nDataLength;
};

Recorder::Recorder(QObject *parent) : QObject(parent)
{

}

void Recorder::setFilePath(const QString &value)
{
    filePath = value;
}

void Recorder::startRecord()
{
    // 判断本地设备是否支持该格式
    QAudioDeviceInfo audioDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    // 判断本地是否有录音设备;
    if (!audioDeviceInfo.isNull())
    {
        destinationFile.setFileName(filePath+".raw");
        destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

        // 设置音频文件格式;
        QAudioFormat format;
        // 设置采样频率;
        format.setSampleRate(simpleRate);
        // 设置通道数;
        format.setChannelCount(channelCount);
        // 设置每次采样得到的样本数据位值;
        format.setSampleSize(simpleSize);
        // 设置编码方法;
        format.setCodec(codeC);
        // 设置采样字节存储顺序;
        format.setByteOrder(QAudioFormat::LittleEndian);
        // 设置采样类型;
        format.setSampleType(QAudioFormat::SignedInt);

        // 判断当前设备设置是否支持该音频格式;
        if (!audioDeviceInfo.isFormatSupported(format))
        {
            qDebug() << "Default format not supported, trying to use the nearest.";
            format = audioDeviceInfo.nearestFormat(format);
        }
        // 开始录音;
        m_audioInput = new QAudioInput(format, this);
        m_audioInput->start(&destinationFile);
    }
    else
    {
        qDebug() << "Current No Record Device";
        // 没有录音设备;
        //                QMessageBox::information(NULL, tr("Record"), tr("Current No Record Device"));
    }
}

void Recorder::stopRecord()
{
    if (m_audioInput != nullptr)
    {
        m_audioInput->stop();
        destinationFile.close();
        delete m_audioInput;
        m_audioInput = nullptr;
    }

    // 将生成的.raw文件转成.wav格式文件;
    addWavHeader(filePath+".raw", filePath+".wav");
}

int Recorder::addWavHeader(QString catheFileName, QString wavFileName)
{
    // 开始准备WAV的文件头
    WAVFILEHEADER WavFileHeader;
    qstrcpy(WavFileHeader.RiffName, "RIFF");
    qstrcpy(WavFileHeader.WavName, "WAVE");
    qstrcpy(WavFileHeader.FmtName, "fmt ");

    WavFileHeader.nFmtLength = 16;  //  表示 FMT 的长度
    WavFileHeader.nAudioFormat = 1; //这个表示 PCM 编码;

    qstrcpy(WavFileHeader.DATANAME, "data");

    WavFileHeader.nBitsPerSample = simpleSize;
    WavFileHeader.nBytesPerSample = simpleSize/8;
    WavFileHeader.nSampleRate = simpleRate;
    WavFileHeader.nBytesPerSecond = simpleRate*simpleSize/8;
    WavFileHeader.nChannleNumber = channelCount;

    QFile cacheFile(catheFileName);
    QFile wavFile(wavFileName);

    if (!cacheFile.open(QIODevice::ReadWrite))
    {
        return -1;
    }
    if (!wavFile.open(QIODevice::WriteOnly))
    {
        return -2;
    }

    int nSize = sizeof(WavFileHeader);
    qint64 nFileLen = cacheFile.bytesAvailable();

    WavFileHeader.nRiffLength = nFileLen - 8 + nSize;
    WavFileHeader.nDataLength = nFileLen;

    // 先将wav文件头信息写入，再将音频数据写入;
    wavFile.write((char *)&WavFileHeader, nSize);
    wavFile.write(cacheFile.readAll());

    cacheFile.close();
    wavFile.close();

    return nFileLen;
}

