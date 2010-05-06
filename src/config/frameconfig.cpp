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
#include "frameconfig.h"
#include "helper.h"
#include "listlayout.h"
#include "framerow.h"

// KDE
#include <kdebug.h>
#include <knuminput.h>
#include <kconfiggroup.h>

// Qt
#include <QtGui/QTreeWidgetItem>


FrameConfig::FrameConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);

    m_layout = new RecordItNow::ListLayout(0, true);
    frameWidgetList->setLayout(m_layout);
    
    addButton->setIcon(KIcon("list-add"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect(sizeEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(m_layout, SIGNAL(removeRequested(QWidget*)), this, SLOT(remove(QWidget*)));
    connect(m_layout, SIGNAL(layoutChanged()), this, SLOT(itemChanged()));
    
    textChanged(QString());

}


QList<FrameSize> FrameConfig::sizes() const
{

    QList<FrameSize> sizes;
    foreach (QWidget *widget, m_layout->rows()) {
        FrameRow *row = static_cast<FrameRow*>(widget);

        sizes.append(FrameSize(row->size(), row->name()));
    }
    return sizes;

}


QList<FrameSize> FrameConfig::defaultSizes()
{

    QList<FrameSize> defaults;

    defaults.append(FrameSize(QSize(640, 480), QString("640 x 480 (4:3 SD)")));
    defaults.append(FrameSize(QSize(800, 600), QString("800 x 600")));
    defaults.append(FrameSize(QSize(1024, 768), QString("1024 x 768")));
    defaults.append(FrameSize(QSize(1280, 720), QString("1280 x 720 (16x9 HD)")));

    return defaults;

}


QList<FrameSize> FrameConfig::readSizes(KConfig *config)
{

    KConfigGroup cfg(config, "Frame");
    return RecordItNow::Helper::listFromArray<FrameSize>(cfg.readEntry("Frames", QByteArray()));

}


void FrameConfig::writeSizes(const QList<FrameSize> &sizes, KConfig *config)
{

    KConfigGroup cfg(config, "Frame");
    cfg.writeEntry("Frames", RecordItNow::Helper::listToArray(sizes));
    cfg.sync();

}


void FrameConfig::saveConfig()
{

    writeSizes(sizes(), config());

}


void FrameConfig::setDefaults()
{

    m_layout->clear();
    foreach (const FrameSize &size, defaultSizes()) {
        FrameRow *row = new FrameRow(this);
        connect(row, SIGNAL(changed()), this, SLOT(itemChanged()));
        
        row->setName(size.text());
        row->setSize(size.size());

        m_layout->addRow(row);
    }

}


void FrameConfig::loadConfig()
{

    foreach (const FrameSize &size, readSizes(config())) {
        FrameRow *row = new FrameRow(this);
        connect(row, SIGNAL(changed()), this, SLOT(itemChanged()));
            
        row->setName(size.text());
        row->setSize(size.size());

        m_layout->addRow(row);
    }

}


void FrameConfig::add()
{

    if (sizeEdit->text().isEmpty()) {
        return;
    }
    
    FrameRow *row = new FrameRow(this);
    connect(row, SIGNAL(changed()), this, SLOT(itemChanged()));

    row->setName(sizeEdit->text());
    row->setSize(QSize(100, 100));

    m_layout->addRow(row);
    sizeEdit->clear();

    setConfigChanged(readSizes(config()) != sizes());

}


void FrameConfig::remove(QWidget *widget)
{

    m_layout->removeRow(widget);
    setConfigChanged(readSizes(config()) != sizes());

}


void FrameConfig::textChanged(const QString &text)
{

    if (text.isEmpty()) {
        addButton->setDisabled(true);
        return;
    }

    foreach (QWidget *widget, m_layout->rows()) {
        FrameRow *row = static_cast<FrameRow*>(widget);
    
        if (row->name() == text) {
            addButton->setDisabled(true);
            return;
        }
    }
    addButton->setDisabled(false);

}


void FrameConfig::itemChanged()
{

    setConfigChanged(readSizes(config()) != sizes());

}


#include "frameconfig.moc"

