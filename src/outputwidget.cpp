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
#include "outputwidget.h"

// KDE
#include <kicon.h>
#include <kdirwatch.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kmenu.h>
#include <kaction.h>
#include <krun.h>
#include <kfileitemactions.h>
#include <kfileitemlistproperties.h>

// Qt
#include <QtCore/QFile>


namespace RecordItNow {


OutputWidget::OutputWidget(QWidget *parent)
    : QFrame(parent)
{

    setupUi(this);

    deleteButton->setIcon(KIcon("list-remove"));
    playButton->setIcon(KIcon("media-playback-start"));

    KAction *openAction = new KAction(this);
    openAction->setText(i18n("Open with..."));
    playButton->addAction(openAction);

    connect(KDirWatch::self(), SIGNAL(created(QString)), this, SLOT(fileCreated(QString)));
    connect(KDirWatch::self(), SIGNAL(deleted(QString)), this, SLOT(fileDeleted(QString)));    
    connect(outputRequester, SIGNAL(textChanged(QString)), this, SLOT(outputFileChangedInternal(QString)));
    connect(playButton, SIGNAL(clicked()), this, SLOT(playOutputFile()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteOutputFile()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openWith()));

}


OutputWidget::~OutputWidget()
{



}


QString OutputWidget::outputFile() const
{

    return outputRequester->text();

}


bool OutputWidget::exists() const
{

    return QFile::exists(outputFile());

}


void OutputWidget::setOutputFile(const QString &file)
{

    outputRequester->setText(file);

}


void OutputWidget::outputFileChangedInternal(const QString &newFile)
{

    if (!m_file.isEmpty()) {
        KDirWatch::self()->removeFile(m_file);
    }
    m_file = newFile;
    if (!m_file.isEmpty()) {
        KDirWatch::self()->addFile(m_file);
    }
    fileDirty(m_file, !exists());

    emit outputFileChanged(newFile);

}


void OutputWidget::fileCreated(const QString &path)
{

    fileDirty(path, false);

}


void OutputWidget::fileDeleted(const QString &path)
{

    fileDirty(path, true);

}


void OutputWidget::fileDirty(const QString &path, const bool &deleted)
{

    if (path != m_file) {
        kWarning() << "wrong file:" << path << "should be:" << m_file;
        return;
    }

    bool enabled = !deleted;
    if (QFileInfo(m_file).isDir()) {
        enabled = false;
    }

    playButton->setEnabled(enabled);
    deleteButton->setEnabled(enabled);

}


void OutputWidget::deleteOutputFile()
{

    if (!exists()) {
        return;
    }

    KIO::SimpleJob *job = KIO::file_delete(KUrl(outputFile()), KIO::HideProgressInfo);
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(deleteFinished(KJob*)));

    job->setAutoDelete(true);
    job->start();

}


void OutputWidget::playOutputFile()
{

    if (!exists()) {
        return;
    }

    new KRun(KUrl(outputFile()), this, true, false);

}


void OutputWidget::deleteFinished(KJob *job)
{

    if (job->error()) {
        emit error(job->errorText());
    }

}


void OutputWidget::openWith()
{

    KRun::displayOpenWithDialog(KUrl::List() << KUrl(outputFile()), this, false);

}



} // namespace RecordItNow


#include "outputwidget.moc"
