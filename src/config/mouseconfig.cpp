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
#include "devicesearchdialog.h"
#include "helper.h"
#include "colorrow.h"
#include "soundrow.h"

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

    m_colorSpacer = new QWidget(this);
    m_colorSpacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    colorWidgetListLayout->addWidget(m_colorSpacer);
    
    m_soundSpacer = new QWidget(this);
    m_soundSpacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    soundWidgetListLayout->addWidget(m_soundSpacer);
    
    addButton->setIcon(KIcon("list-add"));
    soundAddButton->setIcon(KIcon("list-add"));
    keyMonButton->setIcon(KIcon("system-search"));
    volumeSlider->setAudioOutput(RecordItNow::Helper::self()->audioOutput());
    
    connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(soundAddButton, SIGNAL(clicked()), this, SLOT(addSoundClicked()));
    connect(keyMonButton, SIGNAL(clicked()), this, SLOT(showKeyMonDialog()));

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

    connect(kcfg_cursorWidgetSize, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(kcfg_led, SIGNAL(toggled(bool)), this, SLOT(buttonsChanged()));
    connect(kcfg_cursorOpacity, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(kcfg_circle, SIGNAL(toggled(bool)), this, SLOT(modeChanged()));
    connect(kcfg_target, SIGNAL(toggled(bool)), this, SLOT(buttonsChanged()));
    connect(buttonCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentButtonChanged()));
    connect(soundButtonCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentButtonChanged()));
    connect(RecordItNow::Helper::self(), SIGNAL(compositingChanged(bool)), this,
            SLOT(compositingChanged(bool)));

    m_visibleGroup = new QButtonGroup(this);
    m_visibleGroup->addButton(kcfg_mouseWidgetInvisible);
    m_visibleGroup->addButton(kcfg_mouseWidgetAlwaysVisible);
    m_visibleGroup->setExclusive(true);
    
    compositingChanged(RecordItNow::Helper::self()->compositingActive());

    buttonsChanged();
    currentButtonChanged();

}


MouseConfig::~MouseConfig()
{

    delete m_visibleGroup;

}


void MouseConfig::saveConfig()
{

    saveConfig(config(), currentButtons());

}


void MouseConfig::loadConfig()
{

    QList<MouseButton> buttons = getButtons(config());
    foreach (const MouseButton &button, buttons) {

        ColorRow *cRow = newColorRow();
        cRow->setButton(MouseButtonWidget::getButtonFromXButton(button.code()));
        cRow->setColor(button.color());

        addRow(cRow, colorWidgetListLayout);

        if (button.sound().isEmpty()) {
            continue;
        }
            
        SoundRow *sRow = newSoundRow();
        sRow->setButton(MouseButtonWidget::getButtonFromXButton(button.code()));
        sRow->setSound(button.sound());
    
        addRow(sRow, soundWidgetListLayout);
      
    }

    buttonsChanged();
    currentButtonChanged();

}


void MouseConfig::setDefaults()
{

    // clear
    while (colorWidgetListLayout->count() != 1) {
        QWidget *widget = colorWidgetListLayout->itemAt(0)->widget();
        removeRow(widget, colorWidgetListLayout);
    }
    while (soundWidgetListLayout->count() != 1) {
        QWidget *widget = soundWidgetListLayout->itemAt(0)->widget();
        removeRow(widget, soundWidgetListLayout);
    }    
    
    // defaults
    foreach (const MouseButton &button, defaultButtons()) {
        ColorRow *cRow = newColorRow();
        cRow->setButton(MouseButtonWidget::getButtonFromXButton(button.code()));
        cRow->setColor(button.color());

        addRow(cRow, colorWidgetListLayout);
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
    connect(row, SIGNAL(removeRequested()), this, SLOT(removeSoundRow()));
    connect(row, SIGNAL(changed()), this, SLOT(currentButtonChanged()));
    connect(row, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)),
                        this, SLOT(soundButtonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)));

    return row;

}


SoundRow *MouseConfig::newSoundRow()
{

    SoundRow *row = new SoundRow(this);
    connect(row, SIGNAL(removeRequested()), this, SLOT(removeSoundRow()));
    connect(row, SIGNAL(changed()), this, SLOT(currentButtonChanged()));
    connect(row, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)),
                        this, SLOT(soundButtonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)));

    return row;

}


bool MouseConfig::contains(const MouseButtonWidget::Button &button, QLayout *layout, QWidget *exclude) const
{

    for (int i = 0; i < layout->count(); i++) {
        QWidget *widget = layout->itemAt(i)->widget();
        if (widget == m_colorSpacer || widget == m_soundSpacer) {
            continue;
        }
            
        MouseButtonWidget::Button rButton;
        if (layout == colorWidgetListLayout) {
            rButton = static_cast<ColorRow*>(widget)->button();
        } else {
            rButton = static_cast<SoundRow*>(widget)->button();
        }
        
        if (exclude && exclude == widget) {
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
        for (int i = 0; i < colorWidgetListLayout->count(); i++) {
        if (colorWidgetListLayout->itemAt(i)->widget() == m_colorSpacer) {
            continue;
        }
        ColorRow *row = static_cast<ColorRow*>(colorWidgetListLayout->itemAt(i)->widget());

        list.append(MouseButton(row->code(), row->color()));

    }
    
    // sound
    for (int i = 0; i < soundWidgetListLayout->count(); i++) {
        if (soundWidgetListLayout->itemAt(i)->widget() == m_soundSpacer) {
            continue;
        }
        SoundRow *row = static_cast<SoundRow*>(soundWidgetListLayout->itemAt(i)->widget());

        MouseButton button;
        for (int b = 0; b < list.size(); b++) {
            if (row->code() == list.at(b).code()) {
                button = list.takeAt(b);
                break;
            }
        }
        button.setSound(row->sound());
        list.append(button);
    }
 
    return list;

}


void MouseConfig::addRow(QWidget *widget, QLayout *layout)
{

    if (layout == colorWidgetListLayout) {
        colorWidgetListLayout->removeWidget(m_colorSpacer);
        colorWidgetListLayout->addWidget(widget);
        colorWidgetListLayout->addWidget(m_colorSpacer);
    } else {
        soundWidgetListLayout->removeWidget(m_soundSpacer);
        soundWidgetListLayout->addWidget(widget);
        soundWidgetListLayout->addWidget(m_soundSpacer);
    }
    
}
    
    
void MouseConfig::removeRow(QWidget *widget, QLayout *layout)
{

    layout->removeWidget(widget);
    delete widget;

}


void MouseConfig::addClicked()
{

    MouseButtonWidget::Button mButton = MouseButtonWidget::getButtonFromName(buttonCombo->currentText());
    if (mButton == MouseButtonWidget::NoButton) {
        return;
    }

    if (contains(mButton, colorWidgetListLayout)) {
        return;
    }

    ColorRow *row = newColorRow();
    row->setButton(mButton);
    addRow(row, colorWidgetListLayout);
    
    buttonsChanged();

}


void MouseConfig::addSoundClicked()
{

    MouseButtonWidget::Button mButton = MouseButtonWidget::getButtonFromName(soundButtonCombo->currentText());
    if (mButton == MouseButtonWidget::NoButton) {
        return;
    }

    if (contains(mButton, soundWidgetListLayout)) {
        return;
    }
    
    SoundRow *row = newSoundRow();
    row->setButton(mButton);
    addRow(row, soundWidgetListLayout);

    buttonsChanged();

}


void MouseConfig::removeClicked()
{

    ColorRow *row = static_cast<ColorRow*>(sender());

    removeRow(row, colorWidgetListLayout);
    buttonsChanged();

}


void MouseConfig::removeSoundRow()
{
    
    SoundRow *row = static_cast<SoundRow*>(sender());

    removeRow(row, soundWidgetListLayout);
    buttonsChanged();

}


void MouseConfig::buttonChanged(const MouseButtonWidget::Button &oldButton,
                                const MouseButtonWidget::Button &newButton)
{

    ColorRow *changed = static_cast<ColorRow*>(sender());

    if (contains(newButton, colorWidgetListLayout, changed)) {
        KMessageBox::information(this, i18n("The button '%1' has already been defined",
                                            MouseButtonWidget::getName(newButton)));
        changed->setButton(oldButton);
    }
    buttonsChanged();

}


void MouseConfig::soundButtonChanged(const MouseButtonWidget::Button &oldButton, const MouseButtonWidget::Button &newButton)
{

    SoundRow *changed = static_cast<SoundRow*>(sender());
    if (contains(newButton, soundWidgetListLayout, changed)) {
        KMessageBox::information(this, i18n("The button '%1' has already been defined",
                                            MouseButtonWidget::getName(newButton)));
        changed->setButton(oldButton);
    }
    buttonsChanged();

}


void MouseConfig::showKeyMonDialog()
{

    DeviceSearchDialog *dialog = new DeviceSearchDialog(KeyMon::DeviceInfo::MouseType, this);
    connect(dialog, SIGNAL(deviceSelected(QString)), kcfg_mouseDevice, SLOT(setText(QString)));

    dialog->show();

}


void MouseConfig::buttonsChanged()
{

    const QList<MouseButton> buttons = currentButtons();

    cursorWidget->setButtons(buttons);
    cursorWidget->setSize(QSize(kcfg_cursorWidgetSize->value(), kcfg_cursorWidgetSize->value()));

    if (kcfg_led->isChecked()) {
        cursorWidget->setMode(CursorWidget::LEDMode);
    } else if (kcfg_circle->isChecked()) {
        cursorWidget->setMode(CursorWidget::CircleMode);
    } else {
        cursorWidget->setMode(CursorWidget::TargetMode);
    }

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
    if (button == MouseButtonWidget::NoButton || contains(button, colorWidgetListLayout)) {
        addButton->setEnabled(false);
    } else {
        addButton->setEnabled(true);
    }

    // sound
    const MouseButtonWidget::Button button2 = MouseButtonWidget::getButtonFromName(soundButtonCombo->currentText());
    if (button2 == MouseButtonWidget::NoButton || contains(button2, soundWidgetListLayout)) {
        soundAddButton->setEnabled(false);
    } else {
        soundAddButton->setEnabled(true);
    }

    
}


void MouseConfig::compositingChanged(const bool &active)
{

    if (!active) {
        kcfg_circle->setText(i18n("Square"));
        kcfg_cursorOpacity->setEnabled(false);
        opacityLabel->setEnabled(false);
    } else {
        kcfg_circle->setText(i18n("Circle"));
        if (kcfg_circle->isChecked()) {
            kcfg_cursorOpacity->setEnabled(true);
            opacityLabel->setEnabled(true);
        }
    }
    kcfg_target->setEnabled(active);

}


#include "mouseconfig.moc"

