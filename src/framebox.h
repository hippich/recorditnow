#ifndef FRAMEBOX_H
#define FRAMEBOX_H


// own
#include "line.h"

// Qt
#include <QObject>


class QLabel;
class FrameBox : public QObject
{
    Q_OBJECT


public:
    FrameBox(QWidget *parent);
    ~FrameBox();

    QRect boxGeometry() const;
    bool isEnabled() const;

    void setEnabled(const bool &enabled);


private:
    QWidget *m_parent;
    Line *m_left;
    Line *m_top;
    Line *m_right;
    Line *m_bottom;
    QFrame *m_sizeFrame;
    QLabel *m_sizeLabel;
    QTimer *m_timer;
    bool m_enabled;

    void adjustLines();
    void adjustSizeFrame(const QRect &boxGeo);


private slots:
    void lineGeometryChanged(const Line::Side &side, const QRect &geometry);
    void hideFrame();


protected:
    bool eventFilter(QObject *watched, QEvent *event);


};


#endif // FRAMEBOX_H
