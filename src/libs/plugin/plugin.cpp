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
#include "plugin.h"
#include "plugin_p.h"

// KDE
#include <kstandarddirs.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>


namespace RecordItNow {


PluginPrivate::PluginPrivate(Plugin *plugin)
    : QObject(plugin),
    q(plugin)
{


}


PluginPrivate::~PluginPrivate()
{



}


int PluginPrivate::getUniqueId()
{

    int id = 1;
    while (m_uniqueIds.contains(id)) {
        id++;
    }

    m_uniqueIds.append(id);
    return id;

}


void PluginPrivate::removeUniqueId(const int &id)
{

    m_uniqueIds.removeAll(id);

}


void PluginPrivate::jobFinishedInternal(KJob *job)
{

    const int id = jobs.value(job);
    jobs.remove(job);

    QString errorString;
    if (job->error()) {
        errorString = job->errorString();
        if (errorString.isEmpty()) {
            errorString = i18n("Unknown error!");
        }
    }

    removeUniqueId(id);
    q->jobFinished(id, errorString);

}


Plugin::Plugin(const QVariantList &args, QObject *parent)
    : QObject(parent),
    d(new PluginPrivate(this))
{

    Q_ASSERT(!args.isEmpty());
    d->info = args.value(0).value<KPluginInfo>();

}


Plugin::~Plugin()
{

    delete d;

}


KPluginInfo Plugin::info() const
{

    return d->info;

}


int Plugin::move(const QString &from ,const QString &to)
{

    if (!QFile::exists(from) || to.isEmpty()) {
        return -1;
    }

    KIO::FileCopyJob *job = KIO::file_move(KUrl(from), KUrl(to), -1, KIO::HideProgressInfo);
    connect(job, SIGNAL(finished(KJob*)), d, SLOT(jobFinishedInternal(KJob*)));

    job->setAutoDelete(true);

    d->jobs[job] = d->getUniqueId();

    return d->jobs.value(job);

}


int Plugin::remove(const QString &file)
{

    if (!QFile::exists(file)) {
        return -1;
    }

    KIO::SimpleJob *job = KIO::file_delete(KUrl(file), KIO::HideProgressInfo);
    connect(job, SIGNAL(finished(KJob*)), d, SLOT(jobFinishedInternal(KJob*)));

    job->setAutoDelete(true);

    d->jobs[job] = d->getUniqueId();

    return d->jobs.value(job);

}


QString Plugin::getTemporaryFile(const QString &workDir) const
{

    QString tmpDir = workDir;
    if (tmpDir.isEmpty()) {
        tmpDir = KGlobal::dirs()->locateLocal("tmp", "");
    }

    if (tmpDir.isEmpty()) {
        tmpDir = QDir::homePath();
    }

    if (!tmpDir.endsWith('/')) {
        tmpDir.append('/');
    }

    return unique((tmpDir+"recorditnow_tmp"));

}


QString Plugin::unique(const QString &file) const
{

    QString result = file;
    const QRegExp rx("-[0-9]+$");
    const QRegExp frx("\\..{3}$");

    frx.indexIn(result);
    const QString format = frx.cap();
    result.remove(frx);

    while (QFile::exists(result+format)) {
        rx.indexIn(result);
        if (!rx.cap().isEmpty()) {
            int number = rx.cap().remove(0, 1).toInt();
            number++;
            result.remove(rx);
            result += '-';
            result += QString::number(number);
        } else {
            result += "-0";
        }
    }
    result += format;

    return result;

}


void Plugin::jobFinished(const int &id, const QString &errorString)
{

    Q_UNUSED(id);
    Q_UNUSED(errorString);

}


} // namespace RecordItNow


#include "plugin.moc"
#include "plugin_p.moc"
