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


#ifndef RECORDITNOW_LISTWIDGETBUTTONCONTAINER_P_H
#define RECORDITNOW_LISTWIDGETBUTTONCONTAINER_P_H

// own
#include "ui_listwidgetbuttoncontainer.h"
#include "collectionlistwidget.h"

// Qt
#include <QtGui/QListView>


namespace RecordItNow {


class ListWidgetButtonContainer : public QWidget
{
    Q_OBJECT


public:
    explicit ListWidgetButtonContainer(QListView *parent);

    QWidget *button(const RecordItNow::CollectionListWidget::ButtonCode &button) const;
    void setButtons(const RecordItNow::CollectionListWidget::ButtonCodes &buttons);

    void reLayout();


private:
    QHash<CollectionListWidget::ButtonCode, QToolButton*> m_buttons;

    inline QToolButton *newButton(QWidget *parent) const;


protected:
    void resizeEvent(QResizeEvent *event);


signals:
    void sizeChanged();
    void playClicked();
    void addClicked();
    void deleteClicked();
    void editClicked();
    void uploadClicked();


};


} // namespace RecordItNow


#endif // RECORDITNOW_LISTWIDGETBUTTONCONTAINER_P_H
