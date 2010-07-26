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
    setMaxLength(200);

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


void KeyloggerLabel::keyPressEvent(QKeyEvent *event)
{

    kDebug() << "key:" << event->key();
    m_keys.append(event->key());

    QString txt;
    switch (event->key()) {
    case Qt::Key_Left: txt = i18nc("Keyboard key", "Left"); break;
    case Qt::Key_Right: txt = i18nc("Keyboard key", "Right"); break;
    case Qt::Key_Up: txt = i18nc("Keyboard key", "Up"); break;
    case Qt::Key_Down: txt = i18nc("Keyboard key", "Down"); break;
    case Qt::Key_Backspace: txt = i18nc("Keyboard key", "Backspace"); break;
    case Qt::Key_Return: txt = i18nc("Keyboard key", "Return"); break;
    case Qt::Key_Print: txt = i18nc("Keyboard key", "Print"); break;
    case Qt::Key_Tab: txt = i18nc("Keyboard key", "Tab"); break;
    case Qt::Key_Escape: txt = i18nc("Keyboard key", "Escape"); break;
    case Qt::Key_F1: txt = "F1"; break;
    case Qt::Key_F2: txt = "F2"; break;
    case Qt::Key_F3: txt = "F3"; break;
    case Qt::Key_F4: txt = "F4"; break;
    case Qt::Key_F5: txt = "F5"; break;
    case Qt::Key_F6: txt = "F6"; break;
    case Qt::Key_F7: txt = "F7"; break;
    case Qt::Key_F8: txt = "F8"; break;
    case Qt::Key_F9: txt = "F9"; break;
    case Qt::Key_F10: txt = "F10"; break;
    case Qt::Key_F11: txt = "F11"; break;
    case Qt::Key_F12: txt = "F12"; break;
    case Qt::Key_F13: txt = "F13"; break;
    case Qt::Key_F14: txt = "F14"; break;
    case Qt::Key_F15: txt = "F15"; break;
    case Qt::Key_F16: txt = "F16"; break;
    case Qt::Key_F17: txt = "F17"; break;
    case Qt::Key_F18: txt = "F18"; break;
    case Qt::Key_F19: txt = "F19"; break;
    case Qt::Key_F20: txt = "F20"; break;
    case Qt::Key_F21: txt = "F21"; break;
    case Qt::Key_F22: txt = "F22"; break;
    case Qt::Key_F23: txt = "F23"; break;
    case Qt::Key_F24: txt = "F24"; break;
    case Qt::Key_F25: txt = "F25"; break;
    case Qt::Key_F26: txt = "F26"; break;
    case Qt::Key_F27: txt = "F27"; break;
    case Qt::Key_F28: txt = "F28"; break;
    case Qt::Key_F29: txt = "F29"; break;
    case Qt::Key_F30: txt = "F30"; break;
    case Qt::Key_F31: txt = "F31"; break;
    case Qt::Key_F32: txt = "F32"; break;
    case Qt::Key_F33: txt = "F33"; break;
    case Qt::Key_F34: txt = "F34"; break;
    case Qt::Key_F35: txt = "F35"; break;
    case Qt::Key_Insert: txt = i18nc("Keyboard key", "Insert"); break;
    case Qt::Key_End: txt = i18nc("Keyboard key", "End"); break;
    case Qt::Key_PageUp: txt = i18nc("Keyboard key", "Page Up"); break;
    case Qt::Key_PageDown: txt = i18nc("Keyboard key", "Page Down"); break;
    case Qt::Key_Delete: txt = i18nc("Keyboard key", "Delete"); break;
    case Qt::Key_NumLock: txt = i18nc("Keyboard key", "Num Lock"); break;
    case Qt::Key_Space: txt = i18nc("Keyboard key", "Space"); break;
    default: break;
    }

    if (!txt.isEmpty()) {
        txt.prepend('(');
        txt.append(')');
        setText(text()+' '+txt);
        return;
    }

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
