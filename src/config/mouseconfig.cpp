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


// own
#include "mouseconfig.h"
#include "devicesearchdialog.h"
#include "helper.h"

// KDE
#include <kicon.h>
#include <kcolorbutton.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

// Qt
#include <QtGui/QTreeWidget>
#include <QtGui/QToolButton>
#include <QtGui/QButtonGroup>


MouseConfig::MouseConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);

    addButton->setIcon(KIcon("list-add"));
    keyMonButton->setIcon(KIcon("system-search"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(keyMonButton, SIGNAL(clicked()), this, SLOT(showKeyMonDialog()));

    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::LeftButton));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::RightButton));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::MiddleButton));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::WheelUp));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::WheelDown));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::SpecialButton1));
    buttonCombo->addItem(MouseButtonWidget::getName(MouseButtonWidget::SpecialButton2));

    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
    cursorWidget->switchToPreviewMode();

   // connect(this, SIGNAL(configChanged(bool)), this, SLOT(buttonsChanged()));
    connect(kcfg_cursorWidgetSize, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(kcfg_led, SIGNAL(toggled(bool)), this, SLOT(buttonsChanged()));
    connect(kcfg_cursorOpacity, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(kcfg_circle, SIGNAL(toggled(bool)), this, SLOT(modeChanged()));
    connect(kcfg_target, SIGNAL(toggled(bool)), this, SLOT(buttonsChanged()));
    connect(buttonCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentButtonChanged()));
    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(currentButtonChanged()));

    m_visibleGroup = new QButtonGroup(this);
    m_visibleGroup->addButton(kcfg_mouseWidgetInvisible);
    m_visibleGroup->addButton(kcfg_mouseWidgetAlwaysVisible);
    m_visibleGroup->setExclusive(true);

    if (!KWindowSystem::compositingActive()) {
        kcfg_circle->setText(i18n("Square"));
    }

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

        QTreeWidgetItem *item = new QTreeWidgetItem();
        KColorButton *colorButton = newButton();

        colorButton->setColor(button.color());

        MouseButtonWidget *MouseButtonWidget = newMouseButtonWidget();
        MouseButtonWidget->setXButton(button.code());

        treeWidget->addTopLevelItem(item);
        treeWidget->setItemWidget(item, 0, newRemoveButton());
        treeWidget->setItemWidget(item, 1, MouseButtonWidget);
        treeWidget->setItemWidget(item, 2, colorButton);

    }

    buttonsChanged();
    currentButtonChanged();

}


void MouseConfig::setDefaults()
{

    treeWidget->clear();

    foreach (const MouseButton &button, defaultButtons()) {
        MouseButtonWidget *buttonWidget = newMouseButtonWidget();
        buttonWidget->setButton(MouseButtonWidget::getButtonFromXButton(button.code()));

        KColorButton *colorButton = newButton();
        colorButton->setColor(button.color());

        QTreeWidgetItem *item = new QTreeWidgetItem();

        treeWidget->addTopLevelItem(item);
        treeWidget->setItemWidget(item, 0, newRemoveButton());
        treeWidget->setItemWidget(item, 1, buttonWidget);
        treeWidget->setItemWidget(item, 2, colorButton);
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


KColorButton *MouseConfig::newButton()
{

    KColorButton *button = new KColorButton(this);
    connect(button, SIGNAL(changed(QColor)), this, SLOT(buttonsChanged()));
    return button;

}


QToolButton *MouseConfig::newRemoveButton()
{

    QToolButton *remove = new QToolButton(this);
    remove->setMaximumWidth(KIconLoader::SizeMedium);
    remove->setIcon(KIcon("list-remove"));
    connect(remove, SIGNAL(clicked()), this, SLOT(removeClicked()));
    return remove;

}


MouseButtonWidget *MouseConfig::newMouseButtonWidget()
{

    MouseButtonWidget *button = new MouseButtonWidget(this);
    connect(button, SIGNAL(sizeChanged()), this, SLOT(updateColumnSize()));
    connect(button, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)), this,
            SLOT(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)));
    return button;

}


bool MouseConfig::contains(const MouseButtonWidget::Button &button, QWidget *exclude) const
{

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        const MouseButtonWidget *widget = static_cast<MouseButtonWidget*>(treeWidget->itemWidget(item, 1));

        if (exclude && exclude == widget) {
            continue;
        }

        if (widget->getMouseButtonWidget() == button) {
            return true;
        }
    }
    return false;

}


QList<MouseButton> MouseConfig::currentButtons() const
{

    QList<MouseButton> list;
    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        const int button =  static_cast<MouseButtonWidget*>(treeWidget->itemWidget(item, 1))->getXButton();
        const QColor color = static_cast<KColorButton*>(treeWidget->itemWidget(item, 2))->color();

        list.append(MouseButton(button, color));
    }

    return list;

}


void MouseConfig::addClicked()
{

    MouseButtonWidget::Button mButton = MouseButtonWidget::getButtonFromName(buttonCombo->currentText());
    if (mButton == MouseButtonWidget::NoButton) {
        return;
    }

    if (contains(mButton)) {
        return;
    }

    MouseButtonWidget *button = newMouseButtonWidget();
    button->setButton(mButton);

    QTreeWidgetItem *item = new QTreeWidgetItem();

    treeWidget->addTopLevelItem(item);
    treeWidget->setItemWidget(item, 0, newRemoveButton());
    treeWidget->setItemWidget(item, 1, button);
    treeWidget->setItemWidget(item, 2, newButton());

    buttonsChanged();

}


void MouseConfig::removeClicked()
{

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        if (sender() == treeWidget->itemWidget(item, 0)) {
            treeWidget->invisibleRootItem()->removeChild(item);
        }
    }
    buttonsChanged();

}


void MouseConfig::updateColumnSize()
{

    treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);
    treeWidget->header()->setStretchLastSection(true);

}


void MouseConfig::buttonChanged(const MouseButtonWidget::Button &oldButton,
                                const MouseButtonWidget::Button &newButton)
{

    MouseButtonWidget *changed = static_cast<MouseButtonWidget*>(sender());
    if (contains(newButton, changed)) {
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

    if (!KWindowSystem::compositingActive()) {
        kcfg_cursorOpacity->setDisabled(true);
        opacityLabel->setDisabled(true);
        kcfg_target->setDisabled(true);
    } else {
        kcfg_target->setDisabled(false);
    }

}


void MouseConfig::currentButtonChanged()
{

    const MouseButtonWidget::Button button = MouseButtonWidget::getButtonFromName(buttonCombo->currentText());
    if (button == MouseButtonWidget::NoButton || contains(button)) {
        addButton->setEnabled(false);
    } else {
        addButton->setEnabled(true);
    }

}




#include "mouseconfig.moc"

