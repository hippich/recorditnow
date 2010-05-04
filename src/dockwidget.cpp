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
#include "dockwidget.h"

// KDE
#include <kicon.h>
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtCore/QEvent>


namespace RecordItNow {
    
    
DockWidget::DockWidget(QWidget* parent)
    : QDockWidget(parent)
{

    m_window = 0;
    m_title = 0;
    m_widget = 0;

    // title
    QWidget *titleBarWidget = new QWidget(this);
    QHBoxLayout *titleBarLayout = new QHBoxLayout;

    m_title = new QWidget(this);
    QHBoxLayout *titleMainLayout = new QHBoxLayout; // main layout
    QHBoxLayout *titleButtonLayout = new QHBoxLayout; // button layout
    QHBoxLayout *titleBarWidgetLayout = new QHBoxLayout; // user widget layout
    
    titleMainLayout->setContentsMargins(0, 0, 0, 0);
    titleButtonLayout->setContentsMargins(0, 0, 0, 0);
    titleBarWidgetLayout->setContentsMargins(0, 0, 0, 0);
    
    titleMainLayout->addLayout(titleBarWidgetLayout);
    titleMainLayout->addLayout(titleButtonLayout);
    
    m_title->setLayout(titleMainLayout);
    m_buttonLayout = titleButtonLayout;
    
    titleBarLayout->addWidget(m_title);

    titleBarWidget->setLayout(titleBarLayout);
    QDockWidget::setTitleBarWidget(titleBarWidget);

    // content
    QWidget *widget = new QWidget(this);
    QHBoxLayout *widgetLayout = new QHBoxLayout;
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    
    m_widget = new QWidget;
    QHBoxLayout *widgetContentLayout = new QHBoxLayout;
    m_widget->setLayout(widgetContentLayout);
    
    widgetLayout->addWidget(m_widget);

    widget->setLayout(widgetLayout);
    QDockWidget::setWidget(widget);

    connect(this, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), 
           this, SLOT(updateButtons(QDockWidget::DockWidgetFeatures)));

    updateButtons(features());

}

    
DockWidget::~DockWidget()
{

    if (m_window) {
        delete m_window;
    }
    
}
    
    
void DockWidget::setTitleBarWidget(QWidget* widget)
{

    QHBoxLayout *widgetLayout = static_cast<QHBoxLayout*>(m_title->layout()->itemAt(0));
    if (widgetLayout->count() != 0) {
        delete widgetLayout->takeAt(0);
    }
    
    widgetLayout->addWidget(widget);
    
}

    
void DockWidget::setWidget(QWidget* widget)
{

    if (m_widget->layout()->count() != 0) {
        delete m_widget->layout()->takeAt(0);
    }
    m_widget->layout()->addWidget(widget);
    
}


QWidget *DockWidget::widget() const
{

    if (m_widget->layout()->count() > 0) {
        return m_widget->layout()->itemAt(0)->widget();
    } else {
        return 0;
    }
    
}
    

QWidget *DockWidget::titleBarWidget() const
{

    if (m_title->layout()->count() > 0) {
        return m_title->layout()->itemAt(0)->layout()->itemAt(0)->widget();
    } else {
        return 0;
    }
    
}


void DockWidget::setDockTitle(const QString &title)
{

    setWindowTitle(title);
    m_dockTitle = title;

}


void DockWidget::embed()
{

    if (m_window) {
        m_window->layout()->takeAt(0); // m_widget
        m_window->layout()->takeAt(0); // m_title
        
        QDockWidget::widget()->layout()->addWidget(m_widget);
        QDockWidget::titleBarWidget()->layout()->addWidget(m_title);
        
        m_window->deleteLater();
        m_window = 0;
    } else {
        m_window = new QWidget(0, Qt::Window|Qt::WindowStaysOnTopHint|Qt::WindowCloseButtonHint);
        m_window->setWindowTitle(m_dockTitle);
        m_window->setWindowIcon(windowIcon());
        m_window->installEventFilter(this);
        m_window->setAttribute(Qt::WA_DeleteOnClose, false);
        m_window->resize(m_widget->size());
        
        QVBoxLayout *windowLayout = new QVBoxLayout;
        windowLayout->setContentsMargins(0, 0, 0, 0);
        m_window->setLayout(windowLayout);
        
        QDockWidget::widget()->layout()->takeAt(0); // m_widget
        QDockWidget::titleBarWidget()->layout()->takeAt(0); // m_title
        
        windowLayout->addWidget(m_title);
        windowLayout->addWidget(m_widget);
        
        m_window->show();
    }
    updateButtons(features());

}


void DockWidget::updateButtons(const QDockWidget::DockWidgetFeatures &features)
{

    for (int i = m_buttonLayout->count(); i > 0; i--) {
        QLayoutItem *item = m_buttonLayout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    if (m_window) {
        return;
    }
    
    if (features & QDockWidget::DockWidgetFloatable) {
        QToolButton *embedButton = new QToolButton(m_title);
        embedButton->setAutoRaise(true);
        embedButton->setIcon(KIcon("window-new"));
        embedButton->setText(i18n("Embed in window"));
        connect(embedButton, SIGNAL(clicked(bool)), this, SLOT(embed()));

        m_buttonLayout->addWidget(embedButton);
    }
    
    if (features & QDockWidget::DockWidgetClosable) {
        QToolButton *closeButton = new QToolButton(m_title);
        closeButton->setAutoRaise(true);
        closeButton->setIcon(KIcon("window-close"));
        closeButton->setText(i18n("Remove the Dock"));
        connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
        
        m_buttonLayout->addWidget(closeButton);
    }
    
}


bool DockWidget::eventFilter(QObject *watched, QEvent *event)
{

    if (!m_window || watched != m_window) {
        return QDockWidget::eventFilter(watched, event);
    }
    
    if (event->type() == QEvent::Close) {
        embed();
    }
    
    return false;
    
}


void DockWidget::closeEvent(QCloseEvent *event)
{
    
    if (m_window) {
        embed();
    }
    QDockWidget::closeEvent(event);
    
}


} // namespace RecordItNow


#include "dockwidget.moc"


