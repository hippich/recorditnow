/***************************************************************************
 *   Copyright (C) 2010 by Kai Dombrowe <just89@gmx.de>                    *
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

#ifndef RECORDITNOW_OSD_H
#define RECORDITNOW_OSD_H


// Qt
#include <QtGui/QFrame>
#include <QtCore/QVariant>


namespace Plasma {
    class FrameSvg;
}


namespace RecordItNow {


class OSD : public QFrame
{
    Q_OBJECT


public:
    explicit OSD(QWidget *parent = 0);
    ~OSD();

    static QVariant saveGeometry(QWidget *parent, const QRect &geometry);

    void setTransparentForMouseEvents(const bool &transparent);
    void setBackgroundImage(const QString &imagePath);
    void setBackgroundElementPrefix(const QString &prefix);
    void setBlurEnabled(const bool &enabled);
    void loadGeometry(const QVariant &geometry);


private:
    Plasma::FrameSvg *m_background;
    bool m_validBackground;
    bool m_blurEnabled;
    bool m_transparentForMouseEvents;
    QRectF m_osdGeometry;

    void setOSDGeometry(const QRectF &geometry);


private slots:
    void screenGeometryChanged(const int &screen);
    void backgroundChanged();
    void updateBlur();


protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);


};


} // namespace RecordItNow


#endif // RECORDITNOW_OSD_H
