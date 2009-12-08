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
#include "device.h"

// KDE
#include <kdebug.h>
#include <kapplication.h>
#include <kprocess.h>
#include <kstandarddirs.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTimer>


Device::Device(const QString &device, QObject *parent)
    : QObject(parent), m_device(device)
{

    QTimer::singleShot(0, this, SLOT(fix()));

}


Device::~Device()
{


}


void Device::fix()
{

    kDebug() << "fix:" << m_device;

    QFile dev(m_device);
    if (!dev.exists()) {
        exit(Device::ERR_NOSUCHFILE);
        return;
    }

    const QString deviceName = m_device.mid(m_device.lastIndexOf(QDir::separator())).remove(QDir::separator());
    kDebug() << "deviceName:" << deviceName;


    KProcess p;
    p.setOutputChannelMode(KProcess::MergedChannels);

    QString exe;
    QStringList args;
    int ret;

    // create the group
    exe = KGlobal::dirs()->findExe("groupadd");
    if (exe.isEmpty()) {
        exit(Device::ERR_GROUPADDNOTFOUND);
        return;
    }
    args << "-f" << "recorditnow";

    p.setProgram(exe, args);
    p.start();
    p.waitForFinished(-1);
    ret = p.exitCode();

    kDebug() << "ret:" << ret << "ouput:" << p.readAllStandardOutput();

    switch (ret) {
    case 0: break; // success
    case 2: exit(Device::ERR_INTERNALERROR); return; // invalid command syntax
    case 3: exit(Device::ERR_INTERNALERROR); return; // invalid argument to option
    case 4: exit(Device::ERR_INTERNALERROR); return; // GID not unique (when -o not used)
    case 9: exit(Device::ERR_INTERNALERROR); return; // group name not unique
    case 10: exit(Device::ERR_INTERNALERROR); return; // can't update group file
    default: kWarning() << "unkwon return code:" << ret; break;
    }

    // join the group
    const QString user = getenv("USER");
    exe = KGlobal::dirs()->findExe("gpasswd");

    if (exe.isEmpty()) {
        exit(Device::ERR_GPASSWDNOTFOUND);
        return;
    } else if (user.isEmpty()) {
        exit(Device::ERR_NOUSER);
        return;
    }
    args.clear();
    args << "-a" << user << "recorditnow";

    p.setProgram(exe, args);
    p.start();
    p.waitForFinished(-1);
    ret = p.exitCode();

    kDebug() << "ret:" << ret << "ouput:" << p.readAllStandardOutput();

    switch (ret) {
    case 0: break; // success
    default: kWarning() << "unkwon return code:" << ret; break;
    }

    // create udev rules file
    QFile rules("/etc/udev/rules.d/99-recorditnow.rules");
    QFlags<QFile::OpenModeFlag> flags;

    if (rules.exists()) {
        flags = QFile::WriteOnly|QFile::Append;
    } else {
        flags = QFile::WriteOnly;
    }

    if (!rules.open(flags)) {
        kWarning() << "open failed:" << rules.errorString();
        exit(Device::ERR_OPENFAILED);
        return;
    }

    QByteArray data = "KERNEL==\""+deviceName.toLatin1()+"\", MODE=\"660\", GROUP=\"recorditnow\"\n";

    ret = rules.write(data);
    rules.close();
    if (ret == -1) {
        exit(Device::ERR_WRITEFAILED);
        return;
    }

    // chmod for this session
    exe = KGlobal::dirs()->findExe("chmod");

    if (exe.isEmpty()) {
        exit(Device::ERR_CHMODNOTFOUND);
        return;
    }
    args.clear();
    args << "660"  << m_device;

    p.setProgram(exe, args);
    p.start();
    p.waitForFinished(-1);
    ret = p.exitCode();

    kDebug() << "ret:" << ret << "ouput:" << p.readAllStandardOutput();

    switch (ret) {
    case 0: break; // success
    default: kWarning() << "unkwon return code:" << ret; break;
    }

    // success
    exit(Device::ERR_NOERROR);

}


void Device::exit(const Device::Error &code)
{

    kDebug() << "error ?" << code;
    deleteLater();
    kapp->exit(code);

}


