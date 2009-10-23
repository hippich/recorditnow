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

#ifndef ABSTRACTRECORDER_H
#define ABSTRACTRECORDER_H


// KDE
#include <kdemacros.h>
#include <ksharedconfig.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QRect>
#include <QtCore/QVariantList>


struct Data {
public:
    QString outputFile;
    QRect geometry;
    qlonglong winId;
    int fps;
    bool sound;
};


class KDE_EXPORT AbstractRecorder : public QObject
{
    Q_OBJECT


public:
    enum ExitStatus {
        Normal = 0,
        Crash = 1
    };
    AbstractRecorder(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~AbstractRecorder();

    virtual bool canRecordSound() const = 0;


    virtual void record(const Data &) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;


signals:
    void status(const QString &text);
    void finished(const AbstractRecorder::ExitStatus &status);
    void error(const QString &text);


};


#endif // ABSTRACTRECORDER_H
