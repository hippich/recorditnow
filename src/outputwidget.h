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

#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H


// own
#include "ui_outputwidget.h"

// Qt
#include <QtGui/QFrame>


class KJob;
namespace RecordItNow {


class OutputWidget : public QFrame, Ui::OutputWidget
{
    Q_OBJECT


public:
    explicit OutputWidget(QWidget *parent = 0);
    ~OutputWidget();

    QString outputFile() const;
    bool exists() const;


public slots:
    void setOutputFile(const QString &file);
    void deleteOutputFile();
    void playOutputFile();


private:
    QString m_file;


private slots:
    void outputFileChangedInternal(const QString &newFile);
    void fileCreated(const QString &path);
    void fileDeleted(const QString &path);
    void fileDirty(const QString &path, const bool &deleted);
    void deleteFinished(KJob *job);
    void openWith();


signals:
    void outputFileChanged(const QString &newFile);
    void error(const QString &error);


};


} // namespace RecordItNow


#endif // OUTPUTWIDGET_H
