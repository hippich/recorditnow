#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// own
#include "ui_mainwindow.h"
#include "ui_toolbarwidget.h"
#include "libs/recorder/abstractrecorder.h"
#include "ui_settings.h"

// KDE
#include <kxmlguiwindow.h>


class PreviewWidget;
class KNotificationItem;
class InfoDialog;
class KDialog;
class KProcess;
class QStateMachine;
class FrameBox;
class MainWindow : public KXmlGuiWindow, public Ui::MainWindow
{
    Q_OBJECT


public:
    enum State {
        Idle = 0,
        Recording = 1,
        Paused = 2
    };
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    QWidget *m_grabber;
    Ui::ToolBarWidget ui_toolBarWidget;
    Ui::Settings ui_settings;
    FrameBox *m_box;
    AbstractRecorder *m_recorderPlugin;
    QList<AbstractRecorder*> m_recorder;
    KNotificationItem *m_tray;
    QTimer *m_timer;
    Data m_recordData;
    State m_state;

    void setupActions();
    QHash<QString, QString> recorder();
    void initRecorder(Data *d);
    void setupTray();
    void setTrayOverlay(const QString &name);
    void setState(const State &newState);
    inline MainWindow::State state() const;


private slots:
    void startRecord();
    void pauseRecord();
    void stopRecord();

    void recordTriggred();
    void recordWindow();
    void recordCurrentWindow();
    void boxWindow();
    void recordFullScreen();

    void loadRecorder(const QString &name);

    void recorderStatus(const QString &text);
    void recorderError(const QString &error);
    void recorderFinished(const AbstractRecorder::ExitStatus &status);

    void configure();
    void saveConfig(int code);

    void startTimer();
    void tick();
    void lcdUp();
    void lcdDown();

    void trayActivated(const bool &active, const QPoint &pos);


protected:
    bool eventFilter(QObject *watched, QEvent *event);


};


#endif // MAINWINDOW_H
