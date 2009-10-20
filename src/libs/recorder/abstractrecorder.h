#ifndef ABSTRACTRECORDER_H
#define ABSTRACTRECORDER_H


// KDE
#include <kdemacros.h>
#include <ksharedconfig.h>

// Qt
#include <QObject>
#include <QRect>
#include <QVariantList>


struct Data {
public:
    QString outputFile;
    QRect geometry;
    qlonglong winId;
    int fps;
    bool sound;
};


class KDE_EXPORT AbstractRecorder : public QObject
{
    Q_OBJECT


public:
    enum ExitStatus {
        Normal = 0,
        Crash = 1
    };
    AbstractRecorder(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~AbstractRecorder();

    virtual bool canRecordSound() const = 0;
    virtual bool hasConfigPage() const = 0;
    virtual QWidget *configPage() = 0;


    virtual void record(const Data &) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void saveConfig(KSharedConfig::Ptr cfg) = 0;
    virtual void loadConfig(KSharedConfig::Ptr cfg) = 0;


signals:
    void status(const QString &text);
    void finished(const AbstractRecorder::ExitStatus &status);
    void error(const QString &text);


};


#endif // ABSTRACTRECORDER_H
