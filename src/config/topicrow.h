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


#ifndef TOPICROW_H
#define TOPICROW_H


// Qt
#include <QtGui/QWidget>


class KLineEdit;
class KIconButton;
class QTimeEdit;
class TopicRow: public QWidget
{
    Q_OBJECT
    
    
public:
    explicit TopicRow(QWidget *parent = 0);
    
    QString title() const;
    QTime duration() const;
    QString icon() const;
    
    void setTitle(const QString &title);
    void setDuration(const QTime &time);
    void setIcon(const QString &icon);
    
    
private:
    KLineEdit *m_titleEdit;
    KIconButton *m_iconButton;
    QTimeEdit *m_durationEdit;
    

signals:
    void changed();
    
    
};


#endif // TOPICROW_H
