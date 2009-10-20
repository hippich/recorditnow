#ifndef RECORDMYDESKTOPRECORDER_H
#define RECORDMYDESKTOPRECORDER_H


// own
#include "abstractrecorder.h"
#include "ui_recordmydesktop.h"

// Qt
#include <QVariantList>


class KProcess;
class RecordMyDesktopRecorder : public AbstractRecorder
{
    Q_OBJECT


public:
    RecordMyDesktopRecorder(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~RecordMyDesktopRecorder();

    bool canRecordSound() const;
    bool hasConfigPage() const;
    QWidget *configPage();

    void record(const Data &d);
    void pause();
    void stop();
    void saveConfig(KSharedConfig::Ptr cfg);
    void loadConfig(KSharedConfig::Ptr cfg);


private:
    KProcess *m_recorder;
    bool m_paused;
    Ui::RecordMyDesktop ui_cfg;
    QHash<QString, QVariant> m_cfg;


private slots:
    void newRecorderOutput();
    void recorderFinished(int);


};


#endif // RECORDMYDESKTOPRECORDER_H
