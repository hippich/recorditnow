/***************************************************************************
 *   Copyright (C) 2009 by Kai Dombrowe <just89@gmx.de>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef FRAME_H
#define FRAME_H


// own
#include "framesize.h"

// KDE
#include <kdemacros.h>

// Qt
#include <QtGui/QWidget>
#include <QtCore/QHash>


class MoveWidget;
class FrameInfoWidget;
class KDE_EXPORT Frame : public QWidget
{
    Q_OBJECT


public:
    explicit Frame(QWidget *parent = 0);
    ~Frame();

    QRect getFrameGeometry() const;
    bool validFrameGeometry() const;
    void setFrameSize(const FrameSize &size);
    void setVisible(bool visible);
    void setView(int x, int y, int width, int height);
    void setZoomMode(const bool &enabled);
    void setZoomFactor(const int &factor);
    

public slots:
    void setMoveEnabled(const bool &enabled);


private:
    enum ResizeHandle {
        NoHandle = -1,
        Left = 0,
        Top = 1,
        Right = 2,
        Bottom = 3,
        TopLeft = 4,
        TopRight = 5,
        BottomLeft = 6,
        BottomRight = 7
    };
    ResizeHandle m_handle;
    FrameInfoWidget *m_infoWidget;
    MoveWidget *m_moveWidget;
    bool m_active;
    QHash<QString, QSize> m_sizes;
    bool m_validGeometry;

    inline QRect getRect(const ResizeHandle &handle) const;
    inline int getLineSize() const;

    void moveToParent(const bool &force = false);
    void moveParentToFrame();
    void show();
    void hide();


protected:
    bool eventFilter(QObject *watched, QEvent *event);

    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void showEvent(QShowEvent *event);
    void moveEvent(QMoveEvent *event);
    void paintEvent(QPaintEvent *event);


};


#endif // FRAME_H
