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


#ifndef KEYBOARDROW_H
#define KEYBOARDROW_H


// Qt
#include <QtGui/QWidget>


class KIconButton;
class QLabel;
class KeyboardRow : public QWidget
{
    Q_OBJECT
    
    
public:
    explicit KeyboardRow(QWidget *parent = 0);

    
    QString text() const;
    QString icon() const;
    int code() const;

    void setText(const QString &text);
    void setIcon(const QString &icon);
    void setCode(const int &code);
    
    
private:
    KIconButton *m_iconButton;
    QLabel *m_textLabel;
    int m_code;
    
    
signals:
    void changed();
    
    
};


#endif // KEYBOARDROW_H