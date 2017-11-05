#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent = nullptr);

signals:

public slots:
};

#endif // RECORDER_H