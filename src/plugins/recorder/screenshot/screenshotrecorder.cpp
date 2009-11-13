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
#include "screenshotrecorder.h"
#include <recorditnow_screenshot.h>

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>

// Qt
#include <QtGui/QPixmap>
#include <QtGui/QX11Info>
#include <QtCore/QFile>
#include <QtGui/QPainter>
#include <QtCore/QDir>



K_PLUGIN_FACTORY(myFactory, registerPlugin<ScreenshotRecorder>();)
K_EXPORT_PLUGIN(myFactory("screenshot_recorder"))
ScreenshotRecorder::ScreenshotRecorder(QObject *parent, const QVariantList &args)
    : AbstractRecorder(parent, args)
{



}



ScreenshotRecorder::~ScreenshotRecorder()
{



}


bool ScreenshotRecorder::isVideoRecorder() const
{

    return false;

}


void ScreenshotRecorder::record(const AbstractRecorder::Data &d)
{

    setState(Record);

    emit status(i18n("Cheese!"));

    // reload cfg
    Settings::self()->readConfig();

    const int x = d.geometry.x();
    const int y = d.geometry.y();

    const int w = d.geometry.width();
    const int h = d.geometry.height();

    const qlonglong window = d.winId != -1 ? d.winId : QX11Info::appRootWindow();

    QPixmap cheese = QPixmap::grabWindow(window, x, y, w, h);

    QFile outFile(d.outputFile);
    if (outFile.exists()) {
        if (d.overwrite) {
            if (!outFile.remove()) {
                emit error(i18nc("%1 = file, %2 = error string", "screenshot: Remove failed: %1.\n"
                                 "Reason: %2", outFile.fileName(), outFile.errorString()));
                return;
            }
        } else {
            outFile.setFileName(unique(outFile.fileName()));
            emit outputFileChanged(outFile.fileName());
        }
    }

    if (!outFile.open(QIODevice::WriteOnly)) {
        emit error(i18nc("%1 = error string", "Cannot open output file: %1", outFile.errorString()));
        return;
    }

    kDebug() << "format:" << Settings::format();
    if (!cheese.save(&outFile, Settings::format().toUpper().toLatin1())) {
        outFile.close();
        outFile.remove();
        emit error(i18n("Cannot save image."));
        return;
    }
    outFile.close();
    emit finished(AbstractRecorder::Normal);

}


void ScreenshotRecorder::pause()
{



}


void ScreenshotRecorder::stop()
{



}


