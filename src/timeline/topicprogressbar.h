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

#ifndef TopicProgressBar_H
#define TopicProgressBar_H


// own
#include "topic.h"

//KDE
#include <kicon.h>

// Qt
#include <QtGui/QProgressBar>
#include <QtCore/QTime>




class QToolButton;
class QLabel;


namespace RecordItNow {


namespace Timeline {


class TopicProgressBar: public QProgressBar
{
    Q_OBJECT


public:
    TopicProgressBar(QWidget *parent);
    ~TopicProgressBar();

    Topic topic() const;
    void setTopic(const Topic &topic);


private:
    Topic m_topic;


};


} // namespace Timeline


} // namespace RecordItNow


#endif // TopicProgressBar_H