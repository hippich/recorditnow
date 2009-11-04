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

#ifndef RECORDMYDESKTOPRECORDER_H
#define RECORDMYDESKTOPRECORDER_H


// own
#include "abstractrecorder.h"

// Qt
#include <QtCore/QVariantList>


class KProcess;
class RecordMyDesktopRecorder : public AbstractRecorder
{
    Q_OBJECT


public:
    RecordMyDesktopRecorder(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~RecordMyDesktopRecorder();

    bool hasFeature(const AbstractRecorder::Feature &feature) const;
    QString getDefaultOutputFile() const;

    void record(const AbstractRecorder::Data &d);
    void pause();
    void stop();


private:
    KProcess *m_recorder;
    bool m_paused;
    QString m_outputFile;
    QString m_tmpFile;
    bool m_overwrite;

    void clean();

    bool remove(const QString &file);
    bool move(const QString &from, const QString &to);


private slots:
    void newRecorderOutput();
    void recorderFinished(int);


};


#endif // RECORDMYDESKTOPRECORDER_H
