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

#ifndef HELPER_H
#define HELPER_H

// KDE
#include <kglobal.h>

// Qt
#include <QtCore/QList>
#include <QtCore/QDataStream>


namespace Phonon {
    class MediaObject;
    class AudioOutput;
};

class KSelectionWatcher;
namespace RecordItNow {


class MainWindow;
class PluginManager;
class ScriptManager;
class Helper : public QObject
{
    Q_OBJECT


public:
    template <typename T>
    static QByteArray listToArray(const QList<T> &list)
    {

        QByteArray array;
        QDataStream stream(&array, QIODevice::WriteOnly);

        stream << list;

        return array;

    }
    template <typename T>
    static QList<T> listFromArray(const QByteArray &array)
    {

        QDataStream stream(array);

        QList<T> list;
        stream >> list;

        return list;

    }

    static Helper *self();

    bool firstStart() const;
    bool compositingActive() const;
    Phonon::AudioOutput *audioOutput() const;
    PluginManager *pluginmanager() const;
    RecordItNow::ScriptManager *scriptManager() const;
    RecordItNow::MainWindow *window() const;

    void playSound(const QString &file);
    void setMainWindow(RecordItNow::MainWindow *window);
    void unloadScriptManager();


private:
    friend class HelperSingleton;
    Helper();
    ~Helper();

    bool m_firstStart;
    KSelectionWatcher *m_compositeWatcher;
    bool m_compositingActive;
    Phonon::MediaObject *m_audioPlayer;
    Phonon::AudioOutput *m_audioOutput;
    PluginManager *m_pluginManager;
    RecordItNow::ScriptManager *m_scriptManager;
    RecordItNow::MainWindow *m_window;


private slots:
    void compositingChanged();


signals:
    void compositingChanged(const bool &active);


};


} // namespace RecordItNow


#endif // HELPER_H
