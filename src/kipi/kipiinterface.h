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
#ifndef RECORDITNOW_KIPIINTERFACE_H
#define RECORDITNOW_KIPIINTERFACE_H


// KIPI
#include <libkipi/interface.h>
#include <libkipi/imagecollectionshared.h>

// Qt
#include <QtCore/QWeakPointer>


namespace KIPI {
    class PluginLoader;
    class Plugin;
}

class KAction;
namespace RecordItNow {


class KIPIInterface : public KIPI::Interface
{
    Q_OBJECT


public:
    explicit KIPIInterface(QObject *parent = 0);
    ~KIPIInterface();

    KIPI::ImageCollection currentAlbum();
    KIPI::ImageCollection currentSelection();
    QList<KIPI::ImageCollection> allAlbums();
    KIPI::ImageInfo info( const KUrl& url);
    int features() const;
    QList<KAction*> exportPlugins();

    virtual KIPI::ImageCollectionSelector* imageCollectionSelector(QWidget *parent);
    virtual KIPI::UploadWidget* uploadWidget(QWidget *parent);


private:
    KIPI::PluginLoader *m_pluginLoader;
    QList<KIPI::Plugin*> m_plugins;


};


} // namespace RecordItNow


#endif // KIPIINTERFACE_H
