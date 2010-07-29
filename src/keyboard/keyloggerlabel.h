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

#ifndef KeyloggerLabel_H
#define KeyloggerLabel_H


// Qt
#include <QtGui/QWidget>


namespace RecordItNow {


class KeyloggerLabel : public QWidget
{
    Q_OBJECT


public:
    explicit KeyloggerLabel(QWidget *parent = 0);
    ~KeyloggerLabel();

    QString text() const;

    void setShowClipbaord(const bool &enabled);
    void setShowShortcuts(const bool &enabled);
    void setClipboardText(const QString &text);
    void setShortcut(const QString &text);
    void setText(const QString &text);
    void clear();

    void pressEvent(QKeyEvent *event);
    void releaseEvent(QKeyEvent *event);


private:
    QString m_text;
    QString m_shortcut;
    QString m_clipboard;
    QList<int> m_keys;
    QTimer *m_textTimer;
    bool m_showShortcuts;
    bool m_showClipboard;

    QString resizeText(const QString &text) const;


private slots:
    void clearText();


protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);


};


} // namespace RecordItNow


#endif // KeyloggerLabel_H
