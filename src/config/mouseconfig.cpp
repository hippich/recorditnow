/***************************************************************************
 *   Copyright (C) 2009-210 by Kai Dombrowe <just89@gmx.de>                *
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
#include "mouseconfig.h"
#include "helper.h"
#include "colorrow.h"
#include "soundrow.h"
#include "listlayout.h"
#include "keymonmanager.h"

// KDE
#include <kicon.h>
#include <kcolorbutton.h>
#include <kmessagebox.h>
#include <kurlrequester.h>
#include <kstandarddirs.h>
#include <kdebug.h>

// Qt
#include <QtGui/QTreeWidget>
#include <QtGui/QToolButton>
#include <QtGui/QButtonGroup>



MouseConfig::MouseConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{
    
    setupUi(this);

    mouseConfigLayout->addWidget(KeyMonManager::self()->keylogger()->getMouseConfigWidget(this, cfg));

    m_colorLayout = new RecordItNow::ListLayout;
    colorListWidget->setLayout(m_colorLayout);
    
    m_soundLayout = new RecordItNow::ListLayout;
    soundListWidget->setLayout(m_soundLayout);
    
    addButton->setIcon(KIcon("list-add"));
    soundAddButton->setIcon(KIcon("list-add"));
    volumeSlider->setAudioOutput(RecordItNow::Helper::self()->audioOutput());
    
    connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(soundAddButton, SIGNAL(clicked()), this, SLOT(addSoundClicked()));

    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::LeftButton));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::RightButton));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::MiddleButton));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::WheelUp));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::WheelDown));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::SpecialButton1));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::SpecialButton2));

    soundButtonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::LeftButton));
    soundButtonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::RightButton));
    soundButtonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::MiddleButton));
    soundButtonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::WheelUp));
    soundButtonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::WheelDown));
    soundButtonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::SpecialButton1));
    soundButtonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::SpecialButton2));
    
    cursorWidget->switchToPreviewMode();

    connect(m_colorLayout, SIGNAL(removeRequested(QWidget*)), this, SLOT(removeColorRow(QWidget*)));
    connect(m_soundLayout, SIGNAL(removeRequested(QWidget*)), this, SLOT(removeSoundRow(QWidget*)));
    connect(kcfg_cursorWidgetSize, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(kcfg_cursorOpacity, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(appearanceCombo, SIGNAL(activated(int)), this, SLOT(modeActivated()));
    connect(buttonCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentButtonChanged()));
    connect(soundButtonCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentButtonChanged()));
    connect(RecordItNow::Helper::self(), SIGNAL(compositingChanged(bool)), this,
            SLOT(compositingChanged(bool)));
    connect(kcfg_cursorMode, SIGNAL(valueChanged(int)), this, SLOT(loadMode(int)));
    
    compositingChanged(RecordItNow::Helper::self()->compositingActive());

    currentButtonChanged();
    modeActivated();

}


MouseConfig::~MouseConfig()
{


}


void MouseConfig::saveConfig()
{

    saveConfig(config(), currentButtons());
    KeyMonManager::self()->keylogger()->saveConfig(config());

}


void MouseConfig::loadConfig()
{

    QList<MouseButton> buttons = getButtons(config());
    foreach (const MouseButton &button, buttons) {

        ColorRow *cRow = newColorRow();
        cRow->setButton(MouseButtonWidget::getButtonFromXButton(button.code()));
        cRow->setColor(button.color());

        m_colorLayout->addRow(cRow);

        if (button.sound().isEmpty()) {
            continue;
        }
            
        SoundRow *sRow = newSoundRow();
        sRow->setButton(MouseButtonWidget::getButtonFromXButton(button.code()));
        sRow->setSound(button.sound());
    
        m_soundLayout->addRow(sRow);
      
    }

    buttonsChanged();
    currentButtonChanged();

}


void MouseConfig::setDefaults()
{

    // clear

    m_colorLayout->clear();
    m_soundLayout->clear();
    
    // defaults
    foreach (const MouseButton &button, defaultButtons()) {
        ColorRow *cRow = newColorRow();
        cRow->setButton(MouseButtonWidget::getButtonFromXButton(button.code()));
        cRow->setColor(button.color());

        m_colorLayout->addRow(cRow);
    }

}


QList<MouseButton> MouseConfig::defaultButtons()
{

    MouseButton button1;
    MouseButton button3;
    MouseButton button4;
    MouseButton button5;
    MouseButton button8;
    MouseButton button9;

    button1.setCode(MouseButtonWidget::getXButtonFromMouseButton(MouseButtonWidget::LeftButton));
    button3.setCode(MouseButtonWidget::getXButtonFromMouseButton(MouseButtonWidget::RightButton));
    button4.setCode(MouseButtonWidget::getXButtonFromMouseButton(MouseButtonWidget::SpecialButton1));
    button5.setCode(MouseButtonWidget::getXButtonFromMouseButton(MouseButtonWidget::SpecialButton2));
    button8.setCode(MouseButtonWidget::getXButtonFromMouseButton(MouseButtonWidget::WheelDown));
    button9.setCode(MouseButtonWidget::getXButtonFromMouseButton(MouseButtonWidget::WheelUp));

    button1.setColor(Qt::red);
    button3.setColor(Qt::yellow);
    button4.setColor(Qt::darkBlue);
    button5.setColor(Qt::blue);
    button8.setColor(Qt::magenta);
    button9.setColor(Qt::darkMagenta);

    QList<MouseButton> buttons;
    buttons.append(button1);
    buttons.append(button3);
    buttons.append(button4);
    buttons.append(button5);
    buttons.append(button8);
    buttons.append(button9);

    return buttons;

}


QList<MouseButton> MouseConfig::getButtons(KConfig *cfg)
{

    KConfigGroup cfgGroup(cfg, "Mouse");
    return RecordItNow::Helper::listFromArray<MouseButton>(cfgGroup.readEntry("Buttons", QByteArray()));

}


void MouseConfig::saveConfig(KConfig *cfg, const QList<MouseButton> &list)
{

    KConfigGroup cfgGroup(cfg, "Mouse");
    cfgGroup.writeEntry("Buttons", RecordItNow::Helper::listToArray(list));

}


ColorRow *MouseConfig::newColorRow()
{

    ColorRow *row = new ColorRow(this);
    connect(row, SIGNAL(changed()), this, SLOT(buttonsChanged()));
    connect(row, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)),
                        this, SLOT(soundButtonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)));

    return row;

}


SoundRow *MouseConfig::newSoundRow()
{

    SoundRow *row = new SoundRow(this);
    connect(row, SIGNAL(changed()), this, SLOT(buttonsChanged()));
    connect(row, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)),
                        this, SLOT(soundButtonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)));

    return row;

}


bool MouseConfig::contains(const MouseButtonWidget::Button &button, RecordItNow::ListLayout *layout, QWidget *exclude) const
{
    
    foreach (QWidget *row, layout->rows()) {
        MouseButtonWidget::Button rButton;
        if (layout == m_colorLayout) {
            rButton = static_cast<ColorRow*>(row)->button();
        } else {
            rButton = static_cast<SoundRow*>(row)->button();
        }
        
        if (exclude && exclude == row) {
            continue;
        }
        
        if (rButton == button) {
            return true;
        }
    }
    return false;

}


QList<MouseButton> MouseConfig::currentButtons() const
{

    QList<MouseButton> list;
    // color
    foreach (QWidget *widget, m_colorLayout->rows()) {
        ColorRow *row = static_cast<ColorRow*>(widget);
        list.append(MouseButton(row->code(), row->color()));
    }
    
    // sound
    foreach (QWidget *widget, m_soundLayout->rows()) {
        SoundRow *row = static_cast<SoundRow*>(widget);
        
        MouseButton button;
        for (int i = 0; i < list.size(); i++) {
            if (row->code() == list.at(i).code()) {
                button = list.takeAt(i);
                break;
            }
        }
        button.setSound(row->sound());
        list.append(button);
    }
    return list;

}


void MouseConfig::addClicked()
{

    MouseButtonWidget::Button mButton = MouseButtonWidget::getButtonFromName(buttonCombo->currentText());
    if (mButton == MouseButtonWidget::NoButton) {
        return;
    }

    if (contains(mButton, m_colorLayout)) {
        return;
    }

    ColorRow *row = newColorRow();
    row->setButton(mButton);
    m_colorLayout->addRow(row);
    
    buttonsChanged();

}


void MouseConfig::addSoundClicked()
{

    MouseButtonWidget::Button mButton = MouseButtonWidget::getButtonFromName(soundButtonCombo->currentText());
    if (mButton == MouseButtonWidget::NoButton) {
        return;
    }

    if (contains(mButton, m_soundLayout)) {
        return;
    }
    
    SoundRow *row = newSoundRow();
    row->setButton(mButton);
    m_soundLayout->addRow(row);

    buttonsChanged();

}


void MouseConfig::removeColorRow(QWidget *widget)
{

    m_colorLayout->removeRow(widget);
    buttonsChanged();

}


void MouseConfig::removeSoundRow(QWidget *widget)
{
    
    m_soundLayout->removeRow(widget);
    buttonsChanged();

}


void MouseConfig::buttonChanged(const MouseButtonWidget::Button &oldButton,
                                const MouseButtonWidget::Button &newButton)
{

    ColorRow *changed = static_cast<ColorRow*>(sender());
    if (contains(newButton, m_colorLayout, changed)) {
        KMessageBox::information(this, i18n("The button '%1' has already been defined",
                                            MouseButtonWidget::getName(newButton)));
        changed->setButton(oldButton);
    }
    buttonsChanged();

}


void MouseConfig::soundButtonChanged(const MouseButtonWidget::Button &oldButton, const MouseButtonWidget::Button &newButton)
{

    SoundRow *changed = static_cast<SoundRow*>(sender());
    if (contains(newButton, m_soundLayout, changed)) {
        KMessageBox::information(this, i18n("The button '%1' has already been defined",
                                            MouseButtonWidget::getName(newButton)));
        changed->setButton(oldButton);
    }
    buttonsChanged();

}


void MouseConfig::buttonsChanged()
{

    const QList<MouseButton> buttons = currentButtons();

    cursorWidget->setButtons(buttons);
    cursorWidget->setSize(QSize(kcfg_cursorWidgetSize->value(), kcfg_cursorWidgetSize->value()));
    cursorWidget->setMode(currentMode());
    cursorWidget->setOpacity(kcfg_cursorOpacity->value());
    
    currentButtonChanged();
    setConfigChanged(getButtons(config()) != buttons);

}


void MouseConfig::modeChanged()
{

    compositingChanged(RecordItNow::Helper::self()->compositingActive());

}


void MouseConfig::currentButtonChanged()
{

    // color
    const MouseButtonWidget::Button button = MouseButtonWidget::getButtonFromName(buttonCombo->currentText());
    if (button == MouseButtonWidget::NoButton || contains(button, m_colorLayout)) {
        addButton->setEnabled(false);
    } else {
        addButton->setEnabled(true);
    }

    // sound
    const MouseButtonWidget::Button button2 = MouseButtonWidget::getButtonFromName(soundButtonCombo->currentText());
    if (button2 == MouseButtonWidget::NoButton || contains(button2, m_soundLayout)) {
        soundAddButton->setEnabled(false);
    } else {
        soundAddButton->setEnabled(true);
    }
    
}


CursorWidget::WidgetMode MouseConfig::currentMode() const
{

    CursorWidget::WidgetMode mode;
    if (appearanceCombo->currentText() == i18n("LED")) {
        mode = CursorWidget::LEDMode;
    } else if (appearanceCombo->currentText() == i18n("Circle") || 
               appearanceCombo->currentText() == i18n("Square")) {
        mode = CursorWidget::CircleMode;
    } else {
        mode = CursorWidget::TargetMode;
    }

    return mode;

}


void MouseConfig::setMode(const CursorWidget::WidgetMode &mode)
{

    switch (mode) {
    case CursorWidget::LEDMode: 
        appearanceCombo->setCurrentItem(i18n("LED")); 
        break;
    case CursorWidget::CircleMode:
        if (RecordItNow::Helper::self()->compositingActive()) {
            appearanceCombo->setCurrentItem(i18n("Circle")); 
        } else {
            appearanceCombo->setCurrentItem(i18n("Square"));
        }
        break;
    case CursorWidget::TargetMode:
        appearanceCombo->setCurrentItem(i18n("Target"));
        break;
    };
    kcfg_cursorMode->setValue((int)mode);

}


void MouseConfig::updateModeCombo()
{

    appearanceCombo->clear();
    
    appearanceCombo->addItem(i18n("LED"));
    if (RecordItNow::Helper::self()->compositingActive()) {
        appearanceCombo->addItem(i18n("Circle"));
        appearanceCombo->addItem(i18n("Target"));
    } else {
        appearanceCombo->addItem(i18n("Square"));
    }
    
}


void MouseConfig::compositingChanged(const bool &active)
{
    
    CursorWidget::WidgetMode cMode = currentMode();
    updateModeCombo();
    
    if (!active && cMode == CursorWidget::TargetMode) {
        cMode = CursorWidget::LEDMode;
    }
    setMode(cMode);
    
    opacityLabel->setEnabled(active && cMode == CursorWidget::CircleMode);
    kcfg_cursorOpacity->setEnabled(opacityLabel->isEnabled());
      
}


void MouseConfig::modeActivated()
{
    
    buttonsChanged();
    opacityLabel->setEnabled(RecordItNow::Helper::self()->compositingActive() && 
                             currentMode() == CursorWidget::CircleMode);
    kcfg_cursorOpacity->setEnabled(opacityLabel->isEnabled());
 
    kcfg_cursorMode->setValue((int)currentMode());
 
}


void MouseConfig::loadMode(const int &value)
{
    
    CursorWidget::WidgetMode mode = (CursorWidget::WidgetMode) value;
    if (mode != currentMode()) {
        setMode(mode);
    }
        
}


#include "mouseconfig.moc"

