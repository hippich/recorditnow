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


// own
#include "keyloggerlabel.h"

// KDE
#include <plasma/theme.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>



namespace RecordItNow {


KeyloggerLabel::KeyloggerLabel(QWidget *parent)
    : QLineEdit(parent)
{

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}


KeyloggerLabel::~KeyloggerLabel()
{


}


void KeyloggerLabel::pressEvent(QKeyEvent *event)
{

    keyPressEvent(event);

}


void KeyloggerLabel::releaseEvent(QKeyEvent *event)
{

    keyReleaseEvent(event);

}


static bool isModifier(const int &key)
{

    switch (key) {
    case Qt::Key_Shift:
    case Qt::Key_Alt:
    case Qt::Key_Control:
    case Qt::Key_Meta: return true;
    default: return false;
    }

}


void KeyloggerLabel::keyPressEvent(QKeyEvent *event)
{

    m_keys.append(event->key());
    QLineEdit::keyPressEvent(event);
    update();

}


void KeyloggerLabel::keyReleaseEvent(QKeyEvent *event)
{

    m_keys.removeAll(event->key());
    QLineEdit::keyReleaseEvent(event);
    update();

}


void KeyloggerLabel::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.setClipRegion(event->region());
    painter.setPen(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));

    QFont font = Plasma::Theme::defaultTheme()->font(Plasma::Theme::DefaultFont);
    font.setPointSize(height()/2);
    font.setBold(true);
    painter.setFont(font);

    QString text = QLineEdit::text();
    QList<int> tmpKeys = m_keys;


    // shortcut ?
    int modifierCount = 0;
    Qt::KeyboardModifiers modifiers;
    QMutableListIterator<int> it(tmpKeys);
    while (it.hasNext()) {
        it.next();
        switch ((Qt::Key) it.value()) {
        case Qt::Key_Shift: modifiers |= Qt::ShiftModifier; it.remove(); modifierCount++; break;
        case Qt::Key_Control: modifiers |= Qt::ControlModifier; it.remove(); modifierCount++; break;
        case Qt::Key_Meta: modifiers |= Qt::MetaModifier; it.remove(); modifierCount++; break;
        default: break;
        }
    }

    if (modifierCount > 1 ||
        ((modifiers & Qt::ControlModifier || modifiers & Qt::MetaModifier || modifiers & Qt::AltModifier) && !tmpKeys.isEmpty())) {
        int key = 0;
        foreach (const int &k, tmpKeys) {
            key = k;
            break;
        }

        QKeySequence seq(key|modifiers);
        text = seq.toString();
    }

    QFontMetrics fm(font);
    if (fm.width(text) > width()) {
        while (!text.isEmpty()) {
            text.remove(0, 1);

            if (fm.width(text) < width()) {
                break;
            }
        }
    }

    kDebug() << text;

    QTextOption option;
    option.setAlignment(Qt::AlignCenter);
    painter.drawText(rect(), text, option);

}


} // namespace RecordItNow


#include "keyloggerlabel.moc"
