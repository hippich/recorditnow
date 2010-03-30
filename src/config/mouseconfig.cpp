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

    connect(this, SIGNAL(configChanged()), this, SLOT(buttonsChanged()));
    connect(kcfg_cursorWidgetSize, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(kcfg_led, SIGNAL(toggled(bool)), this, SLOT(buttonsChanged()));
    connect(kcfg_cursorOpacity, SIGNAL(valueChanged(int)), this, SLOT(buttonsChanged()));
    connect(kcfg_circle, SIGNAL(toggled(bool)), this, SLOT(modeChanged()));
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

    QList<MouseButton> list;

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        const int button =  static_cast<MouseButtonWidget*>(treeWidget->itemWidget(item, 1))->getXButton();
        const QColor color = static_cast<KColorButton*>(treeWidget->itemWidget(item, 2))->color();

        list.append(MouseButton(button, color));
    }

    saveConfig(config(), list);

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

    if (buttons.isEmpty()) {
        setDefaults();
    }
    buttonsChanged();
    currentButtonChanged();

}


void MouseConfig::setDefaults()
{

    treeWidget->clear();

    MouseButtonWidget *MouseButtonWidget1 = newMouseButtonWidget();
    MouseButtonWidget *MouseButtonWidget3 = newMouseButtonWidget();
    MouseButtonWidget *MouseButtonWidget4 = newMouseButtonWidget();
    MouseButtonWidget *MouseButtonWidget5 = newMouseButtonWidget();
    MouseButtonWidget *MouseButtonWidget8 = newMouseButtonWidget();
    MouseButtonWidget *MouseButtonWidget9 = newMouseButtonWidget();

    MouseButtonWidget1->setButton(MouseButtonWidget::LeftButton);
    MouseButtonWidget3->setButton(MouseButtonWidget::RightButton);
    MouseButtonWidget4->setButton(MouseButtonWidget::WheelUp);
    MouseButtonWidget5->setButton(MouseButtonWidget::WheelDown);
    MouseButtonWidget8->setButton(MouseButtonWidget::SpecialButton1);
    MouseButtonWidget9->setButton(MouseButtonWidget::SpecialButton2);

    KColorButton *button1 = newButton();
    KColorButton *button3 = newButton();
    KColorButton *button4 = newButton();
    KColorButton *button5 = newButton();
    KColorButton *button8 = newButton();
    KColorButton *button9 = newButton();

    button1->setColor(Qt::red);
    button3->setColor(Qt::yellow);
    button4->setColor(Qt::darkBlue);
    button5->setColor(Qt::blue);
    button8->setColor(Qt::magenta);
    button9->setColor(Qt::darkMagenta);

    QTreeWidgetItem *item1 = new QTreeWidgetItem();
    QTreeWidgetItem *item3 = new QTreeWidgetItem();
    QTreeWidgetItem *item4 = new QTreeWidgetItem();
    QTreeWidgetItem *item5 = new QTreeWidgetItem();
    QTreeWidgetItem *item8 = new QTreeWidgetItem();
    QTreeWidgetItem *item9 = new QTreeWidgetItem();

    // left click
    treeWidget->addTopLevelItem(item1);
    treeWidget->setItemWidget(item1, 0, newRemoveButton());
    treeWidget->setItemWidget(item1, 1, MouseButtonWidget1);
    treeWidget->setItemWidget(item1, 2, button1);

    // right click
    treeWidget->addTopLevelItem(item3);
    treeWidget->setItemWidget(item3, 0, newRemoveButton());
    treeWidget->setItemWidget(item3, 1, MouseButtonWidget3);
    treeWidget->setItemWidget(item3, 2, button3);

    // mouse wheel
    treeWidget->addTopLevelItem(item4);
    treeWidget->setItemWidget(item4, 0, newRemoveButton());
    treeWidget->setItemWidget(item4, 1, MouseButtonWidget4);
    treeWidget->setItemWidget(item4, 2, button4);
    treeWidget->addTopLevelItem(item5);
    treeWidget->setItemWidget(item5, 0, newRemoveButton());
    treeWidget->setItemWidget(item5, 1, MouseButtonWidget5);
    treeWidget->setItemWidget(item5, 2, button5);

    // special 1
    treeWidget->addTopLevelItem(item8);
    treeWidget->setItemWidget(item8, 0, newRemoveButton());
    treeWidget->setItemWidget(item8, 1, MouseButtonWidget8);
    treeWidget->setItemWidget(item8, 2, button8);

    // special 2
    treeWidget->addTopLevelItem(item9);
    treeWidget->setItemWidget(item9, 0, newRemoveButton());
    treeWidget->setItemWidget(item9, 1, MouseButtonWidget9);
    treeWidget->setItemWidget(item9, 2, button9);

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
    connect(button, SIGNAL(changed(QColor)), this, SIGNAL(configChanged()));
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

    emit configChanged();

}


void MouseConfig::removeClicked()
{

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        if (sender() == treeWidget->itemWidget(item, 0)) {
            treeWidget->invisibleRootItem()->removeChild(item);
        }
    }
    emit configChanged();

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
    emit configChanged();

}


void MouseConfig::showKeyMonDialog()
{

    DeviceSearchDialog *dialog = new DeviceSearchDialog(KeyMon::DeviceInfo::MouseType, this);
    connect(dialog, SIGNAL(deviceSelected(QString)), kcfg_mouseDevice, SLOT(setText(QString)));

    dialog->show();

}


void MouseConfig::buttonsChanged()
{

    QList<MouseButton> buttons;
    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        const int button =  static_cast<MouseButtonWidget*>(treeWidget->itemWidget(item, 1))->getXButton();
        const QColor color = static_cast<KColorButton*>(treeWidget->itemWidget(item, 2))->color();

        buttons.append(MouseButton(button, color));
    }

    cursorWidget->setButtons(buttons);
    cursorWidget->setSize(QSize(kcfg_cursorWidgetSize->value(), kcfg_cursorWidgetSize->value()));
    cursorWidget->setMode(kcfg_led->isChecked() ? CursorWidget::LEDMode : CursorWidget::CircleMode);
    cursorWidget->setOpacity(kcfg_cursorOpacity->value());

    currentButtonChanged();

}


void MouseConfig::modeChanged()
{

    if (!KWindowSystem::compositingActive()) {
        kcfg_cursorOpacity->setDisabled(true);
        opacityLabel->setDisabled(true);
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

