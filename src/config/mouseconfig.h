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

#ifndef MOUSECONFIG_H
#define MOUSECONFIG_H


// own
#include "config/configpage.h"
#include "ui_mouseconfig.h"
#include "mousebuttonwidget.h"
#include "mouse/mousebutton.h"

// Qt
#include <QtGui/QWidget>


namespace RecordItNow {
    class ListLayout;
};


class ColorRow;
class SoundRow;
class KUrlRequester;
class QButtonGroup;
class QToolButton;
class KColorButton;
class MouseConfig : public RecordItNow::ConfigPage, Ui::MouseConfig
{
    Q_OBJECT


public:
    explicit MouseConfig(KConfig *cfg, QWidget *parent = 0);
    ~MouseConfig();

    void saveConfig();
    void loadConfig();
    void setDefaults();

    static QList<MouseButton> defaultButtons();
    static QList<MouseButton> getButtons(KConfig *cfg);
    static void saveConfig(KConfig *cfg, const QList<MouseButton> &list);


private:
    ColorRow *newColorRow();
    SoundRow *newSoundRow();
    RecordItNow::ListLayout *m_colorLayout;
    RecordItNow::ListLayout *m_soundLayout;

    bool contains(const MouseButtonWidget::Button &button, RecordItNow::ListLayout *layout, QWidget *exclude = 0) const;
    QList<MouseButton> currentButtons() const;
    CursorWidget::WidgetMode currentMode() const;
    
    void addRow(QWidget *widget, QLayout *layout);
    void removeRow(QWidget *widget, QLayout *layout);
    void setMode(const CursorWidget::WidgetMode &mode);
    void updateModeCombo();
    

private slots:
    void addClicked();
    void addSoundClicked();
    void removeColorRow(QWidget *widget);
    void removeSoundRow(QWidget *widget);
    void buttonChanged(const MouseButtonWidget::Button &oldButton, const MouseButtonWidget::Button &newButton);
    void soundButtonChanged(const MouseButtonWidget::Button &oldButton, const MouseButtonWidget::Button &newButton);
    void showKeyMonDialog();
    void buttonsChanged();
    void modeChanged();
    void currentButtonChanged();
    void compositingChanged(const bool &active);
    void modeActivated();
    void loadMode(const int &value);
    

};


#endif // MOUSECONFIG_H
