#ifndef LINE_H
#define LINE_H


// Qt
#include <QtGui/QFrame>



class Line : public QFrame
{
    Q_OBJECT


public:
    enum Side {
        Left = 0,
        Top = 1,
        Right = 2,
        Bottom = 3
    };
    Line(const Line::Side &side);
    ~Line();

    Line::Side side() const;



private:
    Side m_side;
    QWidget *m_parent;



protected:
    void mouseMoveEvent(QMouseEvent *event);


signals:
    void geometryChanged(const Line::Side &side, const QRect &geometry);


};


#endif // LINE_H
